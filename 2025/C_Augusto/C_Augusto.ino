#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;

#define AMOSTRAS 20


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

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
    while (1) {}
  }

  for (int i = 0; i < 10; i++) {
    altitudeTarada += bmp.readAltitude();
  }
  altitudeTarada /= 10.0;

  Serial.println("Temperatura\tPressao\tAltitude\tNivel do mar\tAltitude Filtrada\tAltura");

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
  Serial.println(nomeSD);
} while (SD.exists(nomeSD));

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

  
  if(altura < alturaMAX){
      contagemQUEDA++;
      altitude0ou1 = 0; 
  }
  else if(altura > alturaMAX){
    contagemQUEDA = 0;
    alturaMAX = altura;
    altitude0ou1 = 1;
  }
  

  if(contagemQUEDA == 10){
    Serial.println(altitude0ou1);  
  }
  
  String dataString = "";
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(altitude) + "\t";
  dataString += String(bmp.readSealevelPressure()) + "\t";
  dataString += String(altitudeFiltrada2) + "\t";
  dataString += String(altitudeFiltrada) + "\t";
  dataString += String(altura) + "\t";
  dataString += String(altitude0ou1) + "\t";
  dataString += String(alturaMAX);

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
