#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 53;

Adafruit_BMP085 bmp;
float alt_atual = 0;
float alt_anterior = 0;
float media = 0;
float leitura = 0;
float num_leitura = 0;
float delta = 0;
float soma = 0;
float altitude[10];
int i = 0;
float soma_mm = 0;
float media_movel = 0;
float soma_mm2 = 0;
float altitude2[10];
int j = 0;
float media_movel2 = 0;
float medicao[50];
int queda = 0;
int numA = 0;
int files = 0;
String newData = " ";
File newFile;

void setup() {
  while (!Serial) {
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  String dataString = "";
  while (numA <= files) {
    newData = "bruno" + String(numA) + ".txt";
    numA = numA + 1;
    if (SD.exists("bruno" + String(files) + ".txt")) {
      Serial.println(" This file has already exist");
      Serial.println("error opening datalog.txt");
    }
    else {
      newFile = SD.open("bruno" + String(files) + ".txt");
    }
  }
  
  dataString += String("Average altitude (m)");
  dataString += String("\t");
  dataString += String("Current altidude");
  dataString += String("\t");
  dataString += String("Altitude(m) = ");
  dataString += String("Filtered altitude (m)");
  dataString += String("\t");
  // dataString += String("Real altitude(m) = ");
  //dataString += String("\t");
  Serial.println(dataString);

  if (newFile) {
    newFile.println(dataString);
    newFile.close();
  }
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (i = 0; i < 97; i++) {
    alt_atual =  bmp.readAltitude();
    soma += alt_atual;
  }
  media = soma / 97;
}


void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  alt_atual = bmp.readAltitude() - media;
  for (int i = 9; i > 0; i--) {
    altitude[i] = altitude [i - 1];
  }
  altitude[0] = alt_atual;
  soma_mm = 0;
  for (int i = 0; i < 10; i++) {
    soma_mm += altitude[i];
  }
  media_movel = soma_mm / 10;

  //Serial.print(media_movel);
  //Serial.print("\t");
  dataString += String(media_movel);
  dataString += ("\t");
  //Serial.print(alt_atual);
  //Serial.print("\t");
  dataString += String(alt_atual);
  dataString += ("\t");
  // Serial.print("\t");

  for (int j = 9; j > 0; j--) {
    altitude2[j] = altitude2 [j - 1];
  }
  altitude2[0] = media_movel;
  soma_mm2 = 0;
  for (int j = 0; j < 10; j++) {
    soma_mm2 += altitude2[j];
  }
  media_movel2 = soma_mm2 / 10;
  //Serial.print(media_movel2);
  //Serial.print("\t");
  dataString += String(media_movel2);
  dataString += ("\t");
  //Serial.print(altitude2);
  //Serial.print("\t");

  for (int k = 49; k > 0; k--) {
    medicao[k] = medicao[k - 1];
  }
  medicao[0] = media_movel2;
  queda = 0;
  for (int k = 0; k < 49; k++) {
    if (medicao[k] < medicao[k + 1]) {
      queda += 1;
    }
  }
  //Serial.print(queda/float (50));
  dataString += String(queda / float(50));
  dataString += ("\t");
  // Serial.print("\t");
  if (queda >= 45) {
    //Serial.print("1");
    dataString += String("1");
    dataString += ("\t");
  }
  else {
    //Serial.print("0");
    dataString += String("0");
    dataString += ("\t");
  }
  Serial.println(dataString);

}





//digitalWrite(LED_BUILTIN, HIGH);
//Serial.print(bmp.readPressure());
//Serial.print("\t");
//digitalWrite(LED_BUILTIN, LOW);

// Calculate altitude assuming 'standard' barometric
// pressure of 1013.25 millibar = 101325 Pascal
//Serial.print(bmp.readAltitude());
//Serial.print("\t");

//Serial.print(bmp.readSealevelPressure());
//Serial.print("\t");

//Serial.print(bmp.readAltitude(101500));
//Serial.print("\t");
