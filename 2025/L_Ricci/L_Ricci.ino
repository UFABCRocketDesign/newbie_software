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

#define IGN_1 36
#define IGN_2 61
#define IGN_3 46
#define IGN_4 55
#define LEITURAS 10
#define CHIP_SELECT 53
#define NUMERO_QUEDAS 5

HardwareSerial &LoRa = Serial3;
unsigned long transmissao_lora = 0;

HardwareSerial &GPS = Serial1;
float flat;
float flon;

float alt = 0;
float total = 0;
float total2 = 0;

float leituras[LEITURAS] = {};
float leituras2[LEITURAS] = {};
float altitudes[NUMERO_QUEDAS];

int indiceAtual = 0;
int indiceAtual2 = 0;
int contadorQueda = 0;
int queda = 0;

String zeros;
String filename;
String nome = "LUCAS";
String heading = "Tempo\tTemperatura\tPressão\tAltitudeFiltrada\tAltitudeRaw\tQueda\tParaquedas_1\tParaquedas_2\tAccelX\tAccelY\tAccelZ\tGyroX\tGyroY\tGyroZ\tMagX\tMagY\tMagZ\tLat\tLon";
int incremento = 0;
int tamanho = 0;

float altitudeAnterior = 0;
int altitudeTeto = -3;

int paraquedas_1 = 0;
int paraquedas_2 = 0;
int paraquedas_3 = 0;
int paraquedas_4 = 0;
unsigned long desativacao_p2 = 0;
unsigned long desativacao_p4 = 0;
unsigned long timer_p1;
unsigned long timer_p2;
unsigned long timer_p3;
unsigned long timer_p4;

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

  Serial.println(heading);

  /* Média de Alturas */

  for (int i = 0; i < 10; i++) {
    alt += bmp.readAltitude();
  }
  alt = alt / 10;

  /* Criar ou Abrir Arquivo */

  do {
    tamanho = nome.length() + String(incremento).length();
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

  /* Setar o pinmode dos sensores */

  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
}

void loop() {

  unsigned long timer_lora = millis();

  /* Dados Sensores */

  sensors_event_t event_accel;
  sensors_event_t event_mag;
  accel.getEvent(&event_accel);
  mag.getEvent(&event_mag);
  gyro.read();

  float tempo = millis() / 1000.0;
  float altitudeReal = bmp.readAltitude() - alt;
  float temperatura = bmp.readTemperature();
  float pressao = bmp.readPressure();
  float accel_x = event_accel.acceleration.x;
  float accel_y = event_accel.acceleration.y;
  float accel_z = event_accel.acceleration.z;
  int gyro_x = gyro.g.x;
  int gyro_y = gyro.g.y;
  int gyro_z = gyro.g.z;
  float mag_x = event_mag.magnetic.x;
  float mag_y = event_mag.magnetic.y;
  float mag_z = event_mag.magnetic.z;
  float lat = gps.location.lat();
  float lng = gps.location.lng();

  /* Tratamento de Dados */

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

  /* Imprimindo Dados */

  String dataString = "";
  dataString += String(tempo) + "\t";
  dataString += String(temperatura) + "\t";
  dataString += String(pressao) + "\t";
  dataString += String(media) + "\t";
  dataString += String(mediaNova) + "\t";
  dataString += String(altitudeReal) + "\t";
  dataString += String(queda) + "\t";
  dataString += String(paraquedas_1) + "\t";
  dataString += String(paraquedas_2) + "\t";
  dataString += String(paraquedas_3) + "\t";
  dataString += String(paraquedas_4) + "\t";
  dataString += String(accel_x) + "\t";
  dataString += String(accel_y) + "\t";
  dataString += String(accel_z) + "\t";
  dataString += String(gyro_x) + "\t";
  dataString += String(gyro_y) + "\t";
  dataString += String(gyro_z) + "\t";
  dataString += String(mag_x) + "\t";
  dataString += String(mag_y) + "\t";
  dataString += String(mag_z) + "\t";
  dataString += String(lat, 6) + "\t";
  dataString += String(lng, 6);

  Serial.println(dataString);

  /* Salvando no Cartão SD */

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita.");
  }

  if (timer_lora - transmissao_lora >= 3000) {
    transmissao_lora = timer_lora;
    LoRa.println(dataString);
  }
}