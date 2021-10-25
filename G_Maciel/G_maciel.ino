#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define l 20 // tamanho
#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/

Adafruit_BMP085 bmp;

float novaAlt=0.0;
float velhaAlt=0.0;
float media=0.0;
float h = 0.0;
float lista[l];
float lista2[l];
float media_mov = 0;
float media_mov2 = 0;
const int chipSelect = 53;
String cabecalho = "Altitude [m]\tAltura [m]\tFiltro1 (h)\tFiltro2 (h)\tTemperatura [*C]\tPressao [Pa]\tPressao no nivel do mar [Pa]\tApogeu";
String nomeArquivo = "";

const int ledPin =  LED_BUILTIN;
int ledState = LOW;    // ledState used to set the LED
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated
const long intervalo = 10000;      // interval at which to blink (milliseconds)

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  for (int i=0; i<20; i++) {
    media =  media + bmp.readAltitude();
  }
  media = media / 20;

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized."); 

  String nome = "gabi"; 
  int tamNome = nome.length();
  int num = 0;
  String txt = ".txt";
  bool tmp = false;
  while (tmp == false) {
    String zeros = "";
    int tamNum = String(num).length();
    int tamTotal = tamNome + tamNum;
    for (int j=0; j < (8-tamTotal); j++) {
      zeros = zeros + "0";
    }
    nomeArquivo = nome + zeros + String(num) + txt;
    if (SD.exists(nomeArquivo)) {
      num = num + 1;
      tmp = false;
    }
    else {
      File dataFile = SD.open(nomeArquivo, FILE_WRITE);
      dataFile.println(cabecalho);
      dataFile.close();
      tmp = true;
    }
  }
  Serial.println("file: " + nomeArquivo);
  Serial.println(cabecalho);
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";

  unsigned long currentMillis = millis();
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  float novaAlt=bmp.readAltitude();
  dataString += String(novaAlt);
  dataString += "\t";
  h = novaAlt - media;  //altura 
  dataString += String(h);
  dataString += "\t";
  
  // filtro 1
  for (int k=0; k<(l-1); k++) {
    lista[k] = lista[k+1];
  }
  lista[l-1] = h;
  for (int j=0; j<l; j++) {
    media_mov = media_mov + lista[j];
  }
  media_mov = media_mov/l;
  dataString += String(media_mov);
  dataString += "\t";

  // filtro 2
  for (int k=0; k<(l-1); k++) {
    lista2[k] = lista2[k+1];
  }
  lista2[l-1] = media_mov;
  for (int j=0; j<l; j++) {
    media_mov2 = media_mov2 + lista2[j];
  }
  media_mov2 = media_mov2/l;
  dataString += String(media_mov2);
  dataString += "\t";

  //temperatura
  float temp =  bmp.readTemperature(); 
  dataString += String(temp);
  dataString += "\t";
  //pressão
  float pressao = bmp.readPressure(); 
  dataString += String(pressao);
  dataString += "\t";
  //pressão no nível do mar
  float pressaoNivelMar = bmp.readSealevelPressure(); 
  dataString += String(pressaoNivelMar);
  dataString += "\t";
    
  // encontrando apogeu 
  
  if (h < velhaAlt) {
    dataString += "Apogeu Detectado!";
    previousMillis = millis();
    ledState = HIGH;
  }
  else {
    dataString += " ";   
  }
  if (previousMillis != 0) {
    if (currentMillis - previousMillis >= intervalo) {
      ledState = LOW;
    }
  }
  digitalWrite(ledPin, ledState);

  Serial.println(dataString);
  
  File dataFile = SD.open(nomeArquivo, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  media_mov = 0;
  media_mov2 = 0;
  velhaAlt = h;
}
