#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>]
#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif // ARDUINO_AVR_MEGA2560

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/


Adafruit_BMP085 bmp;
#define N 3
#define L 5
#define H 12
#define maxTamSD 8
File dataFile;
String nome = "leo";
const int chipSelect = 53;
int intervalo = 5000;
int t = millis();
int t1=0;
int paraQued1=0;
bool h;
float med_alt = 0;
float c[N][L];
float vFiltro[N + 1];
float ordH[H];
int k = 0;
float troca = 0;
String docName = "";
int valSd = 0;
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

  if(nome.length()>maxTamSD){
    for(int i=0;i<maxTamSD;i++){
      nome[i] =nome[i+1];
    }
  }

  docName = nome + String(valSd) + ".txt";
  do {
    valSd += 1;
    docName = nome;
    
    int tamN= maxTamSD - nome.length();
    int tamVal = String(valSd).length();
    
    for(int i=0;i<tamN-tamVal;i++){
      docName += String(0);
    }
    
    docName += String(valSd) + ".txt";
  } while(SD.exists(docName));
  
  Serial.println("Creating "+docName+"...");
  dataFile = SD.open(docName, FILE_WRITE);
  cabe += String("Temperature\tPressure\tAltitude\tPressure\tAltitude\t");
  dataFile.println(cabe);
  dataFile.close();


  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  Serial.println(cabe);
  for (int i = 0; i < 10; i++) {
    med_alt += bmp.readAltitude();
  }
  med_alt /= 10;

  pinMode(IGN_1, OUTPUT);
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
  
  for (int i = H - 1; i > 0; i--) {
    ordH[i] = ordH[i - 1];
  }
  ordH[0] = vFiltro[N];
  h = true;
  for (int i = 0; i < H - 1; i++) {
    h = h && (ordH[i] < ordH[i + 1]);

  }

  dataString += String(h);
  dataString += "\t";

  
  if(h && paraQued1==0){
    t1 = t;
    paraQued1 = 1;
    digitalWrite(IGN_1,HIGH);
  } else if(paraQued1 == 1 && t -t1>=intervalo){
    digitalWrite(IGN_1,LOW);
    paraQued1=2;
  }

  dataString += String(paraQued1);
  dataString += "\t";

  Serial.println(dataString);

  File dataFile = SD.open(docName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }


}
