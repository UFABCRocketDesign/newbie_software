#include <Adafruit_BMP085.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <L3G.h>
#include <TinyGPSPlus.h>
Adafruit_BMP085 bmp;
Adafruit_ADXL345_Unified accel;
Adafruit_HMC5883_Unified mag;
TinyGPSPlus gps;
L3G gyro;

#define USANDO_BAROMETRO 1
#define TERMOMETRO 1
#define PRESSAO 1

#define USANDO_ACELEROMETRO 1
#define AX 1
#define AY 1
#define AZ 1

#define USANDO_MAGNETOMETRO 1
#define MX 1
#define MY 1
#define MZ 1

#define USANDO_GIROSCOPIO 1
#define GX 1
#define GY 1
#define GZ 1

#define USANDO_GPS 1
#define GPS_LAT 1
#define GPS_LNG 1

#define USANDO_LORA 1
#define USANDO_CARTAO 1
#define USANDO_CABECALHO 1

#define IGN_1 36
#define IGN_2 61
#define IGN_3 46
#define IGN_4 55

#define LEITURAS 10
#define CHIP_SELECT 53
#define NUMERO_QUEDAS 5

HardwareSerial &LoRa = Serial3;
HardwareSerial &GPS = Serial1;

String heading = "Tempo\tTemperatura\tPressão\tAltitudeFiltrada\tAltitudeRaw\tQueda\tParaquedas_1\tParaquedas_2\tAccelX\tAccelY\tAccelZ\tGyroX\tGyroY\tGyroZ\tMagX\tMagY\tMagZ\tLat\tLon";
String filename;

float alt = 0;
int altitudeTeto = -3;

void setup() {
  Serial.begin(115200);
  LoRa.begin(9600);
  GPS.begin(9600);

  /* Inicializar os Sensores */

  Serial.print("Inicializando cartão SD...");
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("Falha na inicialização do cartão SD!");
    while (1) {}
  }
  Serial.println("Cartão SD inicializado com sucesso.");

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1) {}
  }

  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1) {}
  }

  gyro.enableDefault();

  if (!mag.begin()) {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1) {}
  }

#if USANDO_CABECALHO
  Serial.println(heading);
#endif  // USANDO_CABECALHO

  /* Média de Alturas */


  for (int i = 0; i < 10; i++) {
    alt += bmp.readAltitude();
  }
  alt = alt / 10;

  /* Criar ou Abrir Arquivo */
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

  /* Setar o pinmode dos paraquedas */

  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
}

void loop() {
  unsigned long timer_lora = millis();
  float tempo = millis() / 1000.0;

#if USANDO_BAROMETRO
  float altitudeReal = bmp.readAltitude() - alt;
#if TERMOMETRO
  float temperatura = bmp.readTemperature();
#endif  // TERMOMETRO'
#if PRESSAO
  float pressao = bmp.readPressure();
#endif  // PRESSAO
#endif  // USANDO_BAROMETRO

#if USANDO_ACELEROMETRO
  sensors_event_t event_accel;
  accel.getEvent(&event_accel);
#if AX
  float accel_x = event_accel.acceleration.x;
#endif  //AX
#if AY
  float accel_y = event_accel.acceleration.y;
#endif  // AY
#if AZ
  float accel_z = event_accel.acceleration.z;
#endif  // AZ
#endif  // USANDO_ACELEROMETRO

#if USANDO_GIROSCOPIO
  gyro.read();
#if GX
  int gyro_x = gyro.g.x;
#endif  // GX
#if GY
  int gyro_y = gyro.g.y;
#endif  // GY
#if GZ
  int gyro_z = gyro.g.z;
#endif  // GX
#endif  // USANDO_GIROSCOPIO

#if USANDO_GPS
  while (GPS.available()) {
    gps.encode(GPS.read());
  }
#if GPS_LAT
  float lat = gps.location.lat();
#endif  // GPS_LAT
#if GPS_LNG
  float lng = gps.location.lng();
#endif  // GPS_LNG
#endif  // USANDO_GPS

#if USANDO_MAGNETOMETRO
  sensors_event_t event_mag;
  mag.getEvent(&event_mag);
#if MX
  float mag_x = event_mag.magnetic.x;
#endif  // MX
#if MY
  float mag_y = event_mag.magnetic.y;
#endif  // MY
#if MZ
  float mag_z = event_mag.magnetic.z;
#endif  // MZ
#endif  // USANDO_MAGNETROMETRO

  /* Tratamento de Dados */
#if USANDO_BAROMETRO
  int indiceAtual = 0, indiceAtual2 = 0, contadorQueda = 0, queda = 0;
  int paraquedas_1 = 0, paraquedas_2 = 0, paraquedas_3 = 0, paraquedas_4 = 0;
  unsigned long desativacao_p2 = 0, desativacao_p4 = 0;
  unsigned long timer_p1, timer_p2, timer_p3, timer_p4;
  float total = 0, total2 = 0;
  float leituras[LEITURAS] = {};
  float leituras2[LEITURAS] = {};
  float altitudes[NUMERO_QUEDAS];

  total = total - leituras[indiceAtual];
  leituras[indiceAtual] = (altitudeReal);
  total = total + leituras[indiceAtual];
  indiceAtual = (indiceAtual + 1) % LEITURAS;
  float media = total / LEITURAS;

  total2 = total2 - leituras2[indiceAtual2];
  leituras2[indiceAtual2] = (media);
  total2 = total2 + leituras2[indiceAtual2];
  indiceAtual2 = (indiceAtual2 + 1) % LEITURAS;
  float mediaNova = total2 / LEITURAS;

  float altitudeAnterior = 0;
  if (mediaNova < altitudeAnterior) {
    contadorQueda++;
  } else {
    contadorQueda = 0;
  }
  altitudeAnterior = mediaNova;

  if (contadorQueda >= 10) {
    queda = 1;
  } else {
    queda = 0;
  }

  if (queda == 1 && paraquedas_1 == 0) {
    paraquedas_1 = 1;
    digitalWrite(IGN_1, HIGH);
    timer_p1 = millis();
  }

  if (paraquedas_1 == 1 && (millis() - timer_p1) >= 2000) {
    paraquedas_1 = 2;
    digitalWrite(IGN_1, LOW);
  }

  if (queda == 1 && paraquedas_2 == 0) {
    paraquedas_2 = 1;
    timer_p2 = millis();
  }

  if (paraquedas_2 == 1 && (millis() - timer_p2) >= 2000) {
    paraquedas_2 = 2;
    digitalWrite(IGN_2, HIGH);
    desativacao_p2 = millis();
  }

  if (paraquedas_2 == 2 && (millis() - desativacao_p2) >= 1500) {
    paraquedas_2 = 3;
    digitalWrite(IGN_2, LOW);
  }

  if (queda == 1 && paraquedas_3 == 0 && mediaNova < altitudeTeto) {
    paraquedas_3 = 1;
    digitalWrite(IGN_3, HIGH);
    timer_p3 = millis();
  }

  if (paraquedas_3 == 1 && (millis() - timer_p3) >= 2000) {
    paraquedas_3 = 2;
    digitalWrite(IGN_3, LOW);
  }

  if (queda == 1 && paraquedas_4 == 0 && mediaNova < altitudeTeto) {
    paraquedas_4 = 1;
    timer_p4 = millis();
  }

  if (paraquedas_4 == 1 && (millis() - timer_p4) >= 2000) {
    paraquedas_4 = 2;
    digitalWrite(IGN_4, HIGH);
    desativacao_p4 = millis();
  }

  if (paraquedas_4 == 2 && (millis() - desativacao_p4) >= 1500) {
    paraquedas_4 = 3;
    digitalWrite(IGN_4, LOW);
  }
#endif  // USANDO_BAROMETRO

  /* Imprimindo Dados */

  String dataString = "";

#if USANDO_BAROMETRO
  dataString += String(tempo) + "\t";
#if TERMOMETRO
  dataString += String(temperatura) + "\t";
#endif  // TERMOMETRO
#if PRESSAO
  dataString += String(pressao) + "\t";
#endif  // PRESSAO
  dataString += String(media) + "\t";
  dataString += String(mediaNova) + "\t";
  dataString += String(altitudeReal) + "\t";
  dataString += String(queda) + "\t";
  dataString += String(paraquedas_1) + "\t";
  dataString += String(paraquedas_2) + "\t";
  dataString += String(paraquedas_3) + "\t";
  dataString += String(paraquedas_4) + "\t";
#endif  // USANDO_BAROMETRO

#if USANDO_ACELEROMETRO
#if AX
  dataString += String(accel_x) + "\t";
#endif  // AX
#if AY
  dataString += String(accel_y) + "\t";
#endif  // AY
#if AZ
  dataString += String(accel_z) + "\t";
#endif  // AZz
#endif  // USANDO ACELEROMETRO

#if USANDO_GIROSCOPIO
#if GX
  dataString += String(gyro_x) + "\t";
#endif  // GX
#if GY
  dataString += String(gyro_y) + "\t";
#endif  // GY
#if GZ
  dataString += String(gyro_z) + "\t";
#endif  // GZ
#endif  // USANDO_GIROSCOPIO

#if USANDO_MAGNETOMETRO
#if MX
  dataString += String(mag_x) + "\t";
#endif  // MX
#if MY
  dataString += String(mag_y) + "\t";
#endif  // MY
#if MZ
  dataString += String(mag_z) + "\t";
#endif  // MZ
#endif  // USANDO_MAGNETOMETRO

#if USANDO_GPS
#if GPS_LAT
  dataString += String(lat, 6) + "\t";
#endif  // GPS_LAT
#if GPS_LNG
  dataString += String(lng, 6);
#endif  // GPS_LNG
#endif  // USANDO_GPS

  Serial.println(dataString);

  /* Salvando no Cartão SD */

#if USANDO_CARTAO
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita.");
  }
#endif  // USANDO_CARTAO

#if USANDO_LORA
  unsigned long transmissao_lora = 0;
  if (timer_lora - transmissao_lora >= 3000) {
    transmissao_lora = timer_lora;
    LoRa.println(dataString);
  }
#endif  // USANDO_LORA
}