#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;
const int chipSelect = 53;

float alturainicial;
float altura = 0;
float height[20];
float altufinal;
float somaH;
float somaB;
float filtroA;
float filtroB[20];
float altitude;
float apojas = 0;
float n = 0;
float temperatura;
float pressao;
int j = 0;
int r = 0;
int y = 1;
int apogeu = 0;

int counter = 0;

void setup() {
  Serial.begin(115200);

  // erro de iniciar os sensores

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  while (!Serial) { ; }
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }
  Serial.println("card initialized.");

  String dataString = "";
  dataString += String("Temperatura\tpressão\tAltitude\tpressão em relação ao mar\taltitude real") + "\t";

  String(counter); 
  if ("calvo"+String(counter)+".txt" == 1){
    counter =+ 1;
  }
  File dataFile = SD.open("calvo"+String(counter)+".txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
  // sensores
  for (int i = 0; i < 20; i++) {

    alturainicial = alturainicial + bmp.readAltitude();
  }

  altura = (alturainicial / 20);
}

void loop() {


  altitude = bmp.readAltitude() - altura;
  height[j] = altitude;
  j++;

  if (j >= 20) {
    j = 0;
  }

  somaH = 0;

  for (int k = 0; k < 20; k++) {
    somaH = somaH + height[k];
  }
  filtroA = somaH / 20;

  filtroB[r] = filtroA;
  r++;

  if (r >= 20) {
    r = 0;
  }

  somaB = 0;
  for (int v = 0; v < 20; v++) {
    somaB = somaB + filtroB[v];
  }
  float filtro = somaB / 20;


  if (apojas > filtro) {
    n = n + 0.1;
  } else {
    n = 0;
  }

  if (n >= 20) {
    apogeu = 1;
  }

  apojas = filtro;
  temperatura = bmp.readTemperature();
  pressao = bmp.readPressure();

  File dataFile = SD.open("calvo"+String(counter)+".txt", FILE_WRITE);
  String dataString = "";

  dataString += String(temperatura) + "\t";
  dataString += String(pressao) + "\t";
  dataString += String(altitude) + "\t";
  dataString += String(filtroA) + "\t";
  dataString += String(filtro) + "\t";
  dataString += String(n) + "\t";
  dataString += String(apogeu) + "\t";

  Serial.println(dataString);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
}
