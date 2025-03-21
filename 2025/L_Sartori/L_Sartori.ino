#include <Adafruit_ADXL345_U.h>

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <L3G.h>


#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#include <Adafruit_BMP085.h>

L3G gyro;

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif // ARDUINO_AVR_MEGA2560

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_BMP085 bmp;
#define N 3
#define L 5
#define H 14
#define maxTamSD 8
File dataFile;
String nome = "leo";
const int chipSelect = 53;
int inter1 = 5000;
int interEsp = 2000;
int inter2 = 5000;
int inter3 = 5000;
int inter4 = 5000;
int apoH = -3;
long int t = 0;
long int t1 = 0;
long int t2 = 0;
long int t3 = 0;
long int t4 = 0;
int pQued1 = 0;
int pQued2 = -1;
int pQued3 = 0;
int pQued4 = -1;
bool h;
bool ocoAp = 0;
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
  Wire.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");

  if (nome.length() > maxTamSD) {
    for (int i = 0; i < maxTamSD; i++) {
      nome[i] = nome[i + 1];
    }
  }

  docName = nome + String(valSd) + ".txt";
  do {
    valSd += 1;
    docName = nome;

    int tamN = maxTamSD - nome.length();
    int tamVal = String(valSd).length();

    for (int i = 0; i < tamN - tamVal; i++) {
      docName += String(0);
    }

    docName += String(valSd) + ".txt";
  } while (SD.exists(docName));

  Serial.println("Creating " + docName + "...");
  dataFile = SD.open(docName, FILE_WRITE);
  cabe += String("Temperature\tPressure\tAltitude\tPressure\tAltitude\t");
  dataFile.println(cabe);
  dataFile.close();


  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  if (!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_16_G);

  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
  Serial.println(cabe);
  for (int i = 0; i < 10; i++) {
    med_alt += bmp.readAltitude();
  }
  med_alt /= 10;

  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);

}


void loop() {

  t = millis();
  String dataString = "";

  dataString += String(t / 1000.0);
  dataString += "\t";

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
  if (h) {
    ocoAp = 1;
  }



  if (ocoAp && pQued1 == 0) {
    t1 = t;
    pQued1 = 1;
    digitalWrite(IGN_1, HIGH);
  } else if (pQued1 == 1 && t - t1 >= inter1) {
    digitalWrite(IGN_1, LOW);
    pQued1 = 2;
  }

  if (ocoAp && pQued2 == -1) {
    t2 = t;
    pQued2 = 0;
  } else if (pQued2 == 0 && t - t2 >= interEsp) {
    digitalWrite(IGN_2, HIGH);
    pQued2 = 1;
    t2 = t;
  } else if (pQued2 == 1 && t - t2 >= inter2) {
    digitalWrite(IGN_2, LOW);
    pQued2 = 2;
  }

  if (ocoAp && pQued3 == 0 && apoH >= ordH[0]) {
    t3 = t;
    pQued3 = 1;
    digitalWrite(IGN_3, HIGH);
  } else if (pQued3 == 1 && t - t3 >= inter3) {
    digitalWrite(IGN_3, LOW);
    pQued3 = 2;
  }


  if (ocoAp && pQued4 == -1 && apoH >= ordH[0]) {
    t4 = t;
    pQued4 = 0;
  } else if (pQued4 == 0 && t - t4 >= interEsp) {
    digitalWrite(IGN_4, HIGH);
    pQued4 = 1;
    t4 = t;
  } else if (pQued4 == 1 && t - t4 >= inter4) {
    digitalWrite(IGN_4, LOW);
    pQued4 = 2;
  }



  dataString += String(pQued1);
  dataString += "\t";

  dataString += String(((pQued2 == -1) ? 0 : pQued2));
  dataString += "\t";

  dataString += String(pQued3);
  dataString += "\t";

  dataString += String(pQued4);
  dataString += "\t";


  sensors_event_t event;
  accel.getEvent(&event);

  dataString += String(event.acceleration.x);
  dataString += "\t";

  dataString += String(event.acceleration.y);
  dataString += "\t";

  dataString += String(event.acceleration.z);
  dataString += "\t";

  gyro.read();

  dataString += String((int)gyro.g.x);
  dataString += "\t";

  dataString += String((int)gyro.g.y);
  dataString += "\t";

  dataString += String((int)gyro.g.z);
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
