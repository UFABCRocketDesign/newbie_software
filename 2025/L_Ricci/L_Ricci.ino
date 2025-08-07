#include <SD.h>
#include <Wire.h>
#include <TinyGPSPlus.h>

#include "src/lib/Apogeu/Apogeu.h"
#include "src/lib/Paraquedas/Paraquedas.h"
#include "src/lib/Filtro/Filtro.h"
#include "src/lib/Barometro/BMP085.h"
#include "src/lib/Acelerometro/ADXL345.h"
#include "src/lib/Giroscopio/L3G4200D.h"
#include "src/lib/Magnetrometro/HMC5883L.h"

TinyGPSPlus gps;

#define BARO 1
#define TERMOMETRO 1
#define PRESSAO 1

#define ACEL 1
#define AX 1
#define AY 1
#define AZ 1

#define MAG 1
#define MX 1
#define MY 1
#define MZ 1

#define GIRO 1
#define GX 1
#define GY 1
#define GZ 1

#define USANDO_GPS 1
#define GPS_LAT 1
#define GPS_LNG 1

#define LORA 1
#define SD_CARD 0
#define HEADING 1

#define IGN_1 36
#define IGN_2 61
#define IGN_3 46
#define IGN_4 55

#define CHIP_SELECT 53
#define NUMERO_QUEDAS 5

HardwareSerial &LoRa = Serial3;
HardwareSerial &GPS = Serial1;

String filename;

BMP085 bmp;

ADXL345 accel(2);

L3G4200D gyro;

HMC5883L mag;

Filtro f1(10);
Filtro f2(10);

Paraquedas p1(IGN_1, 0, 1500, false, -3);
Paraquedas p2(IGN_2, 2000, 1500, false, -3);
Paraquedas p3(IGN_3, 0, 1500, true, -3);
Paraquedas p4(IGN_4, 2000, 1500, true, -3);

Apogeu apg;

float altitudes[NUMERO_QUEDAS];
float alt = 0;
int contador = 0;

void readAll() {
  accel.readAll();
  gyro.readAll();
  mag.readAll();
  bmp.readAll(101325.0);

  while (GPS.available()) {
    gps.encode(GPS.read());
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  LoRa.begin(9600);
  GPS.begin(9600);

  /* Inicializar os Sensores */
#if SD_CARD
  Serial.print("Inicializando cartão SD...");
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("Falha na inicialização do cartão SD!");
    while (1) {}
  }
  Serial.println("Cartão SD inicializado com sucesso.");
#endif
#if BARO
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif
#if ACEL
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1) {}
  }
#endif
#if GIRO
  if (!gyro.begin()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1) {}
  }
#endif
#if MAG
  if (!mag.begin()) {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1) {}
  }
#endif

  String heading = "";
  heading += "Tempo\t";
#if TERMOMETRO
  heading += "Temperatura\t";
#endif

#if PRESSAO
  heading += "Pressão\t";
#endif

#if BARO
  heading += "Altitude Filtrada 1\t";
  heading += "Altitude Filtrada 2\t";
  heading += "Altitude Raw\t";
  heading += "Queda\t";
  heading += "Paraquedas 1\t";
  heading += "Paraquedas 2\t";
  heading += "Paraquedas 3\t";
  heading += "Paraquedas 4\t";
#endif

#if ACEL
#if AX
  heading += "Acel X\t";
#endif
#if AY
  heading += "Acel Y\t";
#endif
#if AZ
  heading += "Acel Z\t";
#endif
#endif

#if GIRO
#if GX
  heading += "Gyro X\t";
#endif
#if GY
  heading += "Gyro Y\t";
#endif
#if GZ
  heading += "Gyro Z\t";
#endif
#endif

#if MAG
#if MX
  heading += "Mag X\t";
#endif
#if MY
  heading += "Mag Y\t";
#endif
#if MZ
  heading += "Mag Z\t";
#endif
#endif

#if USANDO_GPS
#if GPS_LAT
  heading += "Lat\t";
#endif
#if GPS_LNG
  heading += "Lng\t";
#endif
#endif

#if HEADING
  Serial.println(heading);
#endif

  /* Média de Alturas */
#if BARO
  for (int i = 0; i < 10; i++) {
    bmp.readAll(101325.0);
    alt += bmp.readAltitude();
  }
  alt = alt / 10;
#endif

  /* Criar ou Abrir Arquivo */
#if SD_CARD
  String nome = "LUCAS";
  String zeros;
  int incremento = 0;
  do {
    int tamanho = nome.length() + String(incremento).length();
    zeros = "";
    for (int i = tamanho; i < 8; i++) {
      zeros += "0";
    }
    filename = nome + zeros + String(incremento) + ".txt";
    incremento++;
  } while (SD.exists(filename));

  Serial.println(filename);

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(heading);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo");
  }
#endif

  /* Setar o pinmode dos paraquedas */

  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
}

void loop() {

  readAll();

  unsigned long timerLora = millis();
  float tempo = millis() / 1000.0;

/* Tratamento de Dados */
#if BARO
  float altitudeReal = bmp.readAltitude() - alt;
  f1.filtro(altitudeReal);
  f2.filtro(f1.getMedia());
  bool queda = apg.detectorQueda(f2.getMedia());

  // Paraquedas
  p1.ativar(f2.getMedia(), queda);
  p2.ativar(f2.getMedia(), queda);
  p3.ativar(f2.getMedia(), queda);
  p4.ativar(f2.getMedia(), queda);
#endif

  /* Imprimindo Dados */

  String dataString = "";

#if BARO
  dataString += String(tempo) + "\t";
#if TERMOMETRO
  dataString += String(bmp.readTemperature()) + "\t";
#endif
#if PRESSAO
  dataString += String(bmp.readPressure()) + "\t";
#endif
  dataString += String(f1.getMedia()) + "\t";
  dataString += String(f2.getMedia()) + "\t";
  dataString += String(altitudeReal) + "\t";
  dataString += String(queda) + "\t";
  dataString += String(p1.getValor()) + "\t";
  dataString += String(p2.getValor()) + "\t";
  dataString += String(p3.getValor()) + "\t";
  dataString += String(p4.getValor()) + "\t";
#endif

#if ACEL
#if AX
  dataString += String(accel.getX()) + "\t";
#endif
#if AY
  dataString += String(accel.getY()) + "\t";
#endif
#if AZ
  dataString += String(accel.getZ()) + "\t";
#endif
#endif

#if GIRO
#if GX
  dataString += String(gyro.getX()) + "\t";
#endif
#if GY
  dataString += String(gyro.getY()) + "\t";
#endif
#if GZ
  dataString += String(gyro.getZ()) + "\t";
#endif
#endif

#if MAG
#if MX
  dataString += String(mag.getX()) + "\t";
#endif
#if MY
  dataString += String(mag.getY()) + "\t";
#endif
#if MZ
  dataString += String(mag.getZ()) + "\t";
#endif
#endif

#if USANDO_GPS
#if GPS_LAT
  dataString += String(gps.location.lat(), 6) + "\t";
#endif
#if GPS_LNG
  dataString += String(gps.location.lng(), 6);
#endif
#endif

  Serial.println(dataString);

  /* Salvando no Cartão SD */

#if SD_CARD
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita.");
  }
#endif

#if LORA
  unsigned long transmissaoLora = 0;
  if (timerLora - transmissaoLora >= 3000) {
    transmissaoLora = timerLora;
    LoRa.println(dataString);
  }
#endif
}