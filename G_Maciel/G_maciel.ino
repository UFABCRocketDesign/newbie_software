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
int encontra_apogeu=0;
int apogeu_detectado = false;
int led_On_Off = 0;      // variavel para entrar no laço LedON e LedOFF apenas 1 vez

const int ledPin =  LED_BUILTIN;
int ledState1 = LOW;    // ledState used to set the LED
int ledState2 = LOW; 
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long ledOnMillis2 = 0;        // quando o led tem que acender
unsigned long ledOffMillis1 = 0; 
unsigned long ledOffMillis2 = 0; // quando o led tem que desligar após o apogeu
const long intervaloOff = 7000;      // interval at which to blink (milliseconds)
const long intervaloOn = 0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
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
  unsigned long t_atual = millis();
  // make a string for assembling the data to log:
  String dataString = "";
  
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

  if (media_mov2 < velhaAlt) {
    encontra_apogeu += 1;
  }
  else {
    encontra_apogeu = 0;
  }

  if (encontra_apogeu == 5) { 
    dataString += "Apogeu Detectado!";
    if (apogeu_detectado == false) {
      ledState1 = HIGH;
      ledOffMillis1 = t_atual + intervaloOff;
      ledOnMillis2 = t_atual + intervaloOn;
      apogeu_detectado = true;
      led_On_Off = 1;
    }
  }

  if (t_atual >= ledOnMillis2 && led_On_Off == 1){
    ledState2 = HIGH;
    ledOffMillis2 = t_atual + intervaloOff;
    led_On_Off = 0;
  }

  if (t_atual >= ledOffMillis1) {
    ledState1 = LOW;
  }
  if (t_atual >= ledOffMillis2) {
    ledState2 = LOW;
  }
  
  digitalWrite(IGN_1, ledState1);
  digitalWrite(IGN_2, ledState2);

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

  velhaAlt = media_mov2;
  media_mov = 0;
  media_mov2 = 0;
}
