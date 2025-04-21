#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;

#define AMOSTRAS 20
#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif // ARDUINO_AVR_MEGA2560

#define IGN_1 36	/*act1*/
#define IGN_2 61	/*act2*/
#define IGN_3 46	/*act3*/
#define IGN_4 55	/*act4*/

#define HEAL_1 68	/*health 1*/
#define HEAL_2 62	/*health 2*/
#define HEAL_3 56	/*health 3*/
#define HEAL_4 58	/*health 4*/

#define BUZZ_PIN A0
#define BUZZ_CMD LOW

HardwareSerial &LoRa(Serial3);
HardwareSerial &GPS(Serial1);

const int chipSelect = 53;
float leituras[AMOSTRAS];
float leituras2[AMOSTRAS];
float soma = 0;
float soma2 = 0;
int indice = 0;
int indice1 = 0; 
float altitudeTarada = 0;
char nomeSD[15];
int contagemSD = 0;
int contagemQUEDA = 0;
float alturaMAX = 0;
String dataString = "";
int altitude0ou1 = 0;
unsigned long tempoantigo = 0;
unsigned long tempoantigo2 = 0;
unsigned long tempoantigo3 = 0;
unsigned long temponovo = 0;
const long intervalo = 1000;
int paraquedascontador = 0;
int tempoparaquedas = 0;
int alturaparaquedas = 0;
int IGNSTATUS = LOW;
int IGNSTATUS2 = LOW;
int IGNSTATUS3 = LOW;
int desativarign = 0;
int desativarign2 = 0;
int desativarign3 = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
    while (1) {}
  }

  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  //pinMode(IGN_4,OUTPUT);

  for (int i = 0; i < 10; i++) {
    altitudeTarada += bmp.readAltitude();
  }
  altitudeTarada /= 10.0;

  Serial.println("Temperatura\tPressao\tAltitude\tNivel do mar\tAltitude Filtrada\tAltura\tParaquedas\tTempo");

  while (!Serial);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }
  
  File dataFile = SD.open(nomeSD, FILE_WRITE);
  
do {
  if (contagemSD == 0) {
    sprintf(nomeSD, "dataC.txt");
  } else {
    sprintf(nomeSD, "dataC%03d.txt", contagemSD);
  }
  contagemSD++;
  
} while (SD.exists(nomeSD));

Serial.println(nomeSD);

dataFile = SD.open(nomeSD, FILE_WRITE);

if (dataFile) {
  dataFile.println("Temperatura\tPressao\tAltitude\tNivel do mar\tAltitude Filtrada\tAltura\t");
  dataFile.close();
} else {
  Serial.print("Erro ao abrir o arquivo: ");
  Serial.println(nomeSD);
}

Serial.println("initialization done.");

}

void loop() {

  float altitude = bmp.readAltitude();
  float altura = altitude - altitudeTarada;
  
  soma = soma - leituras[indice] + altura;
  leituras[indice] = altura;
  indice = (indice + 1) % AMOSTRAS;

  float altitudeFiltrada = soma / AMOSTRAS;
  
  soma2 = soma2 - leituras2[indice1] + altitudeFiltrada;
  leituras2[indice1] = altitudeFiltrada;
  indice1 = (indice1 + 1) % AMOSTRAS;
  float altitudeFiltrada2 = soma2 / AMOSTRAS;

  
  if(altitudeFiltrada2 < alturaMAX){
      contagemQUEDA++; 
  }
  else if(altitudeFiltrada2 > alturaMAX){
    contagemQUEDA = 0;
    alturaMAX = altitudeFiltrada2;
  }
  
    if(contagemQUEDA >= 10){
      altitude0ou1 = 1;  
    }else if (contagemQUEDA > 10 || contagemQUEDA < 10){
      altitude0ou1 = 0;
    }
  
  
  unsigned long tempoatual = millis();

  if (altitude0ou1 == 1 && desativarign == 0){
    IGNSTATUS = HIGH;
    digitalWrite(IGN_1,IGNSTATUS);
    tempoantigo = tempoatual;
    desativarign = 1;
  }
  if (tempoatual - tempoantigo >= 10000 && desativarign == 1){
    IGNSTATUS = LOW;
    digitalWrite(IGN_1,IGNSTATUS);
    desativarign = 2;
  }

  
  if (altitude0ou1 == 1 && desativarign2 == 0){
    tempoantigo2 = tempoatual;
    desativarign2 = 1;
  }
  if (altitude0ou1 == 1 && tempoatual - tempoantigo2 >= 4000 && desativarign2 == 1){
    IGNSTATUS2 = HIGH;
    digitalWrite(IGN_2, IGNSTATUS2);
    desativarign2 = 2;
    temponovo = tempoatual;
  }

  if (tempoatual - temponovo >= 2000 && desativarign2 == 2){
    IGNSTATUS2 = LOW;
    digitalWrite(IGN_2,IGNSTATUS2);
    desativarign2 = 3;
  }

  if(altitudeFiltrada2 <= -3 && altitude0ou1 == 1 && desativarign3 == 0){
    tempoantigo3 = tempoatual;
    IGNSTATUS3 = HIGH;
    digitalWrite(IGN_3,IGNSTATUS3);
    desativarign3 = 1;
  }

if(desativarign3 == 1 && tempoatual - tempoantigo3 >= 3000){
  IGNSTATUS3 = LOW;
  digitalWrite(IGN_3,IGNSTATUS3);
}
  

  String dataString = "";
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(altitude) + "\t";
  dataString += String(altitudeFiltrada2) + "\t";
  dataString += String(altitudeFiltrada) + "\t";
  dataString += String(altura) + "\t";
  dataString += String(altitude0ou1) + "\t";
  dataString += String(alturaMAX) + "\t";
  dataString += String(tempoatual) + "\t";
  dataString += String(desativarign) + "\t";
  dataString += String(desativarign2) + "\t";
  dataString += String(desativarign3) + "\t";
  
  Serial.println(dataString);

  File dataFile = SD.open(nomeSD, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening:");
    Serial.println(nomeSD);
  }
  
}
