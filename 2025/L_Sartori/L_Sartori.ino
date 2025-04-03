#include <SPI.h>
#include <SD.h>
#include <Wire.h>


#define GPS_HABILITAR 1
#define LORA_HABILITAR 1
#define ACCEL_HABILITAR 1
#define ACCEL_X_HABILITAR (ACCEL_HABILITAR && 1)
#define ACCEL_Y_HABILITAR (ACCEL_HABILITAR && 1)
#define ACCEL_Z_HABILITAR (ACCEL_HABILITAR && 1)
#define MAG_HABILITAR 1
#define MAG_X_HABILITAR (MAG_HABILITAR && 1)
#define MAG_Y_HABILITAR (MAG_HABILITAR && 1)
#define MAG_Z_HABILITAR (MAG_HABILITAR && 1)
#define GYRO_HABILITAR 1
#define GYRO_X_HABILITAR (GYRO_HABILITAR && 1)
#define GYRO_Y_HABILITAR (GYRO_HABILITAR && 1)
#define GYRO_Z_HABILITAR (GYRO_HABILITAR && 1)
#define BMP_HABILITAR 1
#define TEMP_HABILITAR (BMP_HABILITAR && 1)
#define BARO_HABILITAR (BMP_HABILITAR && 1)
#define PRESS_HABILITAR (BMP_HABILITAR && 1)
#define PQUEDAS_HABILITAR (BARO_HABILITAR && 1)
#define CHIP_HABILITAR 0

#if GPS_HABILITAR
HardwareSerial &GPS = Serial1;
#include <TinyGPSPlus.h>
TinyGPSPlus gps;
#endif

#include <Adafruit_Sensor.h>

#if LORA_HABILITAR
HardwareSerial &LoRa(Serial3);
#define loraEsp 3000
long int tLora=0;
#endif

#if ACCEL_HABILITAR
#include <Adafruit_ADXL345_U.h>
#endif

#if MAG_HABILITAR
#include <Adafruit_HMC5883_U.h>
#endif

#if BMP_HABILITAR
#include <Adafruit_BMP085.h>
#endif

#if GYRO_HABILITAR
#include <L3G.h>
#endif

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif  // ARDUINO_AVR_MEGA2560

#if PQUEDAS_HABILITAR
#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/
#define inter1 5000
#define interEsp 2000
#define inter2 5000
#define inter3 5000
#define inter4 5000
#define apoH -3
#endif

#if CHIP_HABILITAR
#define chipSelect 53
#define maxTamSD 8
#endif

#if BARO_HABILITAR
#define N 3
#define L 5
#define H 14
#define valMedH 10
#endif

#if CHIP_HABILITAR
File dataFile;
String docName = "";
#endif

#if MAG_HABILITAR
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
#endif
#if ACCEL_HABILITAR
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
#endif
#if BMP_HABILITAR
Adafruit_BMP085 bmp;
#endif
#if GYRO_HABILITAR
L3G gyro;
#endif 

#if BMP_HABILITAR
float med_alt = 0;
int k = 0;
#endif

#if PQUEDAS_HABILITAR
  long int t1 = 0;
  long int t2 = 0;
  long int t3 = 0;
  long int t4 = 0;
  int pQued1 = 0;
  int pQued2 = -1;
  int pQued3 = 0;
  int pQued4 = -1;
  bool ocoAp = 0;
#endif

#if BARO_HABILITAR
  bool h;
  float valoresFiltros[N][L];
  float vFiltro[N + 1];
  float ordH[H];
#endif

void setup() {
  String cabe = "";
#if CHIP_HABILITAR
  String nome = "leo";
  int valSd = 0;
#endif
  Serial.begin(115200);
#if LORA_HABILITAR
  LoRa.begin(9600);
#endif
#if GPS_HABILITAR
  GPS.begin(9600);
#endif
  Wire.begin();
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
#if CHIP_HABILITAR
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }
#endif
#if BMP_HABILITAR
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif
#if ACCEL_HABILITAR
  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif
#if GYRO_HABILITAR
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }
#endif
#if MAG_HABILITAR
  if (!mag.begin()) {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif
#if CHIP_HABILITAR
  Serial.println("card initialized.");

  if (nome.length() > maxTamSD) {
    for (int i = 0; i < maxTamSD; i++) {
      nome[i] = nome[i + 1];
    }
  }


  do {
    docName = nome;

    for (int i = 0; i < (maxTamSD - nome.length()) - (String(valSd).length()); i++) {
      docName += String(0);
    }

    docName += String(valSd) + ".txt";

    valSd += 1;
  } while (SD.exists(docName));

  Serial.println("Creating " + docName + "...");
  dataFile = SD.open(docName, FILE_WRITE);
#endif 

  cabe += String("tempo\t");

#if TEMP_HABILITAR
  cabe += String("Temperature\t");
#endif
#if PRESS_HABILITAR
  cabe += String("Pressure\t");
#endif
#if BARO_HABILITAR
  for(int i=0;i<N+1;i++){
    cabe += String("Filtro");
    cabe += String(i+1) +"\t";
  }
  cabe += String("h\t");
#endif
#if PQUEDAS_HABILITAR
  cabe += String("pQuedas1\tpQuedas2\tpQuedas3\tpQuedas4\t");
#endif
#if ACCEL_X_HABILITAR
  cabe += String("acc_x\t");
#endif
#if ACCEL_Y_HABILITAR
  cabe += String("acc_y\t");
#endif
#if ACCEL_Z_HABILITAR
  cabe += String("acc_z\t");
#endif
#if GYRO_X_HABILITAR
  cabe += String("gyro_x\t");
#endif
#if GYRO_Y_HABILITAR
  cabe += String("gyro_y\t");
#endif
#if GYRO_Z_HABILITAR
  cabe += String("gyro_z\t");
#endif
#if MAG_X_HABILITAR
  cabe += String("mag_x\t");
#endif
#if MAG_Y_HABILITAR
  cabe += String("mag_y\t");
#endif
#if MAG_Z_HABILITAR
  cabe += String("mag_z\t");
#endif
#if GPS_HABILITAR
  cabe += String("latitude\tlongitude\t");
#endif
#if CHIP_HABILITAR
  dataFile.println(cabe);
  dataFile.close();
#endif
  Serial.println(cabe);
#if LORA_HABILITAR
  LoRa.println(cabe);
#endif
#if BARO_HABILITAR
  for (int i = 0; i < valMedH; i++) {
    med_alt += bmp.readAltitude();
  }
  med_alt /= valMedH;
#endif
#if PQUEDAS_HABILITAR
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
#endif
#if ACCEL_HABILITAR
  accel.setRange(ADXL345_RANGE_16_G);
#endif
#if GYRO_HABILITAR
  gyro.enableDefault();
#endif
}


void loop() {

  long int t = millis();

  String dataString = "";
#if ACCEL_HABILITAR
  sensors_event_t eventac;
  accel.getEvent(&eventac);
#endif
#if GYRO_HABILITAR
  gyro.read();
#endif
#if MAG_HABILITAR
  sensors_event_t eventmag;
  mag.getEvent(&eventmag);
#endif
#if BARO_HABILITAR
  vFiltro[0] = bmp.readAltitude() - med_alt;

  for (int i = 0; i < N; i++) {
    valoresFiltros[i][k] = vFiltro[i];
    vFiltro[i + 1] = 0;
    for (int j = 0; j < L; j++) {
      vFiltro[i + 1] += valoresFiltros[i][j];
    }
    vFiltro[i + 1] /= L;
  }

  k += 1;
  k %= L;


  for (int i = H - 1; i > 0; i--) {
    ordH[i] = ordH[i - 1];
  }
  ordH[0] = vFiltro[N];
  h = true;
  for (int i = 0; i < H - 1; i++) {
    h = h && (ordH[i] < ordH[i + 1]);
  }
#endif
#if PQUEDAS_HABILITAR
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
#endif 

  dataString += String(t / 1000.0);
  dataString += "\t";

#if TEMP_HABILITAR
  dataString += String(bmp.readTemperature());
  dataString += "\t";
#endif
#if PRESS_HABILITAR
  dataString += String(bmp.readPressure());
  dataString += "\t";
#endif
#if BARO_HABILITAR
  for (int i = 0; i < N + 1; i++) {
    dataString += String(vFiltro[i]);
    dataString += "\t";
  }

  dataString += String(h);
  dataString += "\t";
#endif
#if PQUEDAS_HABILITAR
  dataString += String(pQued1);
  dataString += "\t";

  dataString += String(pQued2);
  dataString += "\t";

  dataString += String(pQued3);
  dataString += "\t";

  dataString += String(pQued4);
  dataString += "\t";
#endif
#if ACCEL_X_HABILITAR
  dataString += String(eventac.acceleration.x);
  dataString += "\t";
#endif
#if ACCEL_Y_HABILITAR
  dataString += String(eventac.acceleration.y);
  dataString += "\t";
#endif
#if ACCEL_Z_HABILITAR
  dataString += String(eventac.acceleration.z);
  dataString += "\t";
#endif
#if GYRO_X_HABILITAR
  dataString += String((int)gyro.g.x);
  dataString += "\t";
#endif
#if GYRO_Y_HABILITAR
  dataString += String((int)gyro.g.y);
  dataString += "\t";
#endif
#if GYRO_Z_HABILITAR
  dataString += String((int)gyro.g.z);
  dataString += "\t";
#endif 
#if MAG_X_HABILITAR
  dataString += String(eventmag.magnetic.x);
  dataString += "\t";
#endif
#if MAG_Y_HABILITAR
  dataString += String(eventmag.magnetic.y);
  dataString += "\t";
#endif
#if MAG_Z_HABILITAR
  dataString += String(eventmag.magnetic.z);
  dataString += "\t";
#endif
#if GPS_HABILITAR
  while (GPS.available()){
    gps.encode(GPS.read());
  }
  dataString += String(gps.location.lat(), 6);
  dataString += "\t";

  dataString += String(gps.location.lng(), 6);
  dataString += "\t";
#endif
  Serial.println(dataString);
#if LORA_HABILITAR
  if(t-tLora>=loraEsp){
    LoRa.println(dataString);
    tLora = t;
  }
#endif

#if CHIP_HABILITAR
  

  File dataFile = SD.open(docName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
#endif
}
