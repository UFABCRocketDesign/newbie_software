#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

#define l 20 // tamanho
#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/
#define h_paraquedas_2 10  // altura para acionar 2º paraquedas (led2)
#define ledPin LED_BUILTIN
#define interv_desliga_led 7000      // interval at which to blink (milliseconds)
#define interv_liga_led2 4000

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
String cabecalho = "";
String nomeArquivo = "";
int encontra_apogeu=0;
int apogeu_detectado = false;
int laco_led_2 = false;      // variavel para entrar no laço liga led 2 
int laco_led_3 = false;      // variavel para entrar no laço liga led 3 (built in)
String mag_X;
String mag_Y;
String mag_Z;


int ledState1 = LOW;    // ledState used to set the LED
int ledState2 = LOW; 
int ledState3 = LOW;
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long liga_led2 = 0;        // quando o led tem que acender
unsigned long desliga_led1 = 0; 
unsigned long desliga_led2 = 0; // quando o led tem que desligar após o apogeu
unsigned long desliga_led3 = 0;

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
    /* Initialise the mag sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
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

  cabecalho += "Altitude [m]\tAltura [m]\tFiltro1 (h)\tFiltro2 (h)";
  cabecalho += "\tTemperatura [*C]";
  cabecalho += "\tPressao [Pa]\tPressao no nivel do mar [Pa]";
  cabecalho += "\tApogeu";
  cabecalho += "\tMag eixo X [uT]\t Mag eixo Y [uT]\tMag eixo Z [uT]";

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
    dataString += "Apogeu Detectado! Led 1";
    if (apogeu_detectado == false) {
      // LED 1
      ledState1 = HIGH;
      desliga_led1 = t_atual + interv_desliga_led;
      liga_led2 = t_atual + interv_liga_led2;
      apogeu_detectado = true;
      laco_led_2 = true;
      laco_led_3 = true;
    }
  }
  // LED 2
  if (t_atual >= liga_led2 && laco_led_2 == true){
    ledState2 = HIGH;
    dataString += "\t";
    dataString += "Led 2!";
    desliga_led2 = t_atual + interv_desliga_led;
    laco_led_2 = false;
  }
  // LED 3
  if (media_mov2 <= h_paraquedas_2 && laco_led_3 == true) {
    ledState3 = HIGH;
    dataString += "\t";
    dataString += "Led 3!";
    desliga_led3 = t_atual + interv_desliga_led;
    laco_led_3 = false;
  }

  if (t_atual >= desliga_led1) {
    ledState1 = LOW;
  }
  if (t_atual >= desliga_led2) {
    ledState2 = LOW;
  }
  if (t_atual >= desliga_led3) {
    ledState3 = LOW;
  }
  
  digitalWrite(IGN_1, ledState1);
  digitalWrite(IGN_2, ledState2);
  digitalWrite(ledPin, ledState3);

  // Magnetometro
  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);
  mag_X = event.magnetic.x;
  dataString += "\t";
  dataString += String(mag_X);
  mag_Y = event.magnetic.y;
  dataString += "\t";
  dataString += String(mag_Y);
  mag_Z = event.magnetic.z;
  dataString += "\t";
  dataString += String(mag_Z);
 
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
