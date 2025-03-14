#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;
#define N 3
#define L 5
#define H 4
const int chipSelect = 53;
bool h;
float med_alt = 0;
float c[N][L];
float vFiltro[N + 1];
float ordH[H];
int k = 0;
float troca = 0;
String nome[] = "leo";
int valSd=0;
void setup() {
  String cabe = "";
  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");

  if (nome.length()> 7){
    Serial.println("Nome muito grande");
  } else{
    int tamN = 8 - nome.length();
  }
  int maxSd = 1;
  for (int i=0;i<n;i++){
    maxSd *= 10;
  }
  String svalSd = Integer.toString(valSd);
  while(SD.exists(nome+svalSd+".txt")==1 && valSd < maxSd){
    valSd+=1;
    svalSd = Integer.toString(valSd);
  }
  Serial.println("Creating example.txt...");
  myFile = SD.open(nome+valSd+".txt", FILE_WRITE);
  myFile.close();
  

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  cabe += String("Temperature\tPressure\tAltitude\tPressure\tAltitude\t");
  Serial.println(cabe);
  for (int i = 0; i < 10; i++) {
    med_alt += bmp.readAltitude();
  }
  med_alt /= 10;
}


void loop() {

  String dataString = "";

  dataString += String(bmp.readTemperature());
  dataString += "\t";

  dataString += String(bmp.readPressure());
  dataString += "\t";

  vFiltro[0] = bmp.readAltitude() - med_alt;

  for (int i = 0 ; i < N; i++) {
    c[i][k] = vFiltro[i];
    vFiltro[i + 1] = 0;
    for (int j = 0; j < L; j++) {
      vFiltro[i + 1] += c[i][j];
    }
    vFiltro[i + 1] /= 5;
  }

  k += 1;
  k %= 5;
  for (int i = 0; i < N + 1; i++) {
    dataString += String(vFiltro[i]);
    dataString += "\t";
  }

  dataString += String(bmp.readSealevelPressure());
  dataString += "\t";

  for (int i = H - 1; i > 0; i--) {
    ordH[i] = ordH[i - 1];
  }
  ordH[0] = vFiltro[N];
  h = true;
  for (int i = 0; i < H - 1; i++) {
    h = h && ordH[i] < ordH[i + 1];

  }

  dataString += String(h);
  dataString += "\t";

  dataString += String(bmp.readAltitude(101500));
  dataString += "\t";

  Serial.println(dataString);

  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }


}
