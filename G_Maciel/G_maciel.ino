#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define l 20 // tamanho
Adafruit_BMP085 bmp;

float novaAlt=0.0;
float media=0.0;
float h = 0.0;
float lista[l];
float lista2[l];
float media_mov = 0;
float media_mov2 = 0;
const int chipSelect = 53;
String cabecalho = "Altura [m]\tFiltro 1\tFiltro 2";
// float velhaAlt=0.0;
File dataFile = SD.open("gabriela.txt", FILE_WRITE);

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  // pinMode(LED_BUILTIN, OUTPUT);
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
  Serial.println(cabecalho); 
 
  dataFile;
  dataFile.println(cabecalho);
  dataFile.close();
  
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  float novaAlt=bmp.readAltitude();
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

  Serial.println(dataString);
  // Serial.print("\t");
  //if (h < velhaAlt) {
    //Serial.println("caindo");
    //velhaAlt = h;
  //}
  //if (h > velhaAlt) {
    //Serial.println("subindo");
    //velhaAlt = h;
  //}

  // open the file.
  dataFile;

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
}
