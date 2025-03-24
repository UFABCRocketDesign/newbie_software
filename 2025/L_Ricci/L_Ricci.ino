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
#define SD_CARD 1
#define HEADING 1

#define IGN_1 36
#define IGN_2 61
#define IGN_3 46
#define IGN_4 55

#define LEITURAS 10
#define FILTROS 2
#define CHIP_SELECT 53
#define NUMERO_QUEDAS 5
#define ALTITUDE_TETO -3

HardwareSerial &LoRa = Serial3;
HardwareSerial &GPS = Serial1;

String filename;

#if BARO
int paraquedas_1 = 0, paraquedas_2 = 0, paraquedas_3 = 0, paraquedas_4 = 0, contadorQueda = 0, queda = 0;
unsigned long desativacao_p2 = 0, desativacao_p4 = 0;
unsigned long timer_p1, timer_p2, timer_p3, timer_p4;
float leituras[FILTROS][LEITURAS] = {};
float total[FILTROS] = {};
int indiceAtual[FILTROS] = {};
float altitudes[NUMERO_QUEDAS];
float alt = 0;
#endif

void setup() {
  Serial.begin(115200);
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
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1) {}
  }

  gyro.enableDefault();
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
  heading += "Altitude Filtrada\t";
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

float filtros(float altitudeReal, int i) {
#if BARO
  total[i] = total[i] - leituras[i][indiceAtual[i]];
  leituras[i][indiceAtual[i]] = (altitudeReal);
  total[i] = total[i] + leituras[i][indiceAtual[i]];
  indiceAtual[i] = (indiceAtual[i] + 1) % LEITURAS;
  float altura = total[i] / LEITURAS;

  return altura;
#endif
}

int detector_queda(float altura) {
#if BARO
  float altitudeAnterior = 0;

  if (altura < altitudeAnterior) {
    contadorQueda++;
  } else {
    contadorQueda = 0;
  }

  altitudeAnterior = altura;

  if (contadorQueda >= 10) {
    queda = 1;
  } else {
    queda = 0;
  }
  return queda;
#endif
}

int paraquedas1(float altura, int queda) {
#if BARO
  if (queda == 1 && paraquedas_1 == 0) {
    paraquedas_1 = 1;
    digitalWrite(IGN_1, HIGH);
    timer_p1 = millis();
  }

  if (paraquedas_1 == 1 && (millis() - timer_p1) >= 2000) {
    paraquedas_1 = 2;
    digitalWrite(IGN_1, LOW);
  }
  return paraquedas_1;
#endif
}

int paraquedas2(float altura, int queda) {
#if BARO
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
  return paraquedas_2;
#endif
}

int paraquedas3(float altura, int queda) {
#if BARO
  if (queda == 1 && paraquedas_3 == 0 && altura < ALTITUDE_TETO) {
    paraquedas_3 = 1;
    digitalWrite(IGN_3, HIGH);
    timer_p3 = millis();
  }

  if (paraquedas_3 == 1 && (millis() - timer_p3) >= 2000) {
    paraquedas_3 = 2;
    digitalWrite(IGN_3, LOW);
  }
  return paraquedas_3;
#endif
}

int paraquedas4(float altura, int queda) {
#if BARO
  if (queda == 1 && paraquedas_4 == 0 && altura < ALTITUDE_TETO) {
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
  return paraquedas_4;
#endif
}

void loop() {
  unsigned long timer_lora = millis();
  float tempo = millis() / 1000.0;

#if BARO
  float altitudeReal = bmp.readAltitude() - alt;
#if TERMOMETRO
  float temperatura = bmp.readTemperature();
#endif
#if PRESSAO
  float pressao = bmp.readPressure();
#endif
#endif

#if ACEL
  sensors_event_t event_accel;
  accel.getEvent(&event_accel);
#if AX
  float accel_x = event_accel.acceleration.x;
#endif
#if AY
  float accel_y = event_accel.acceleration.y;
#endif
#if AZ
  float accel_z = event_accel.acceleration.z;
#endif
#endif

#if GIRO
  gyro.read();
#if GX
  int gyro_x = gyro.g.x;
#endif
#if GY
  int gyro_y = gyro.g.y;
#endif
#if GZ
  int gyro_z = gyro.g.z;
#endif
#endif

#if USANDO_GPS
  while (GPS.available()) {
    gps.encode(GPS.read());
  }
#if GPS_LAT
  float lat = gps.location.lat();
#endif
#if GPS_LNG
  float lng = gps.location.lng();
#endif
#endif

#if MAG
  sensors_event_t event_mag;
  mag.getEvent(&event_mag);
#if MX
  float mag_x = event_mag.magnetic.x;
#endif
#if MY
  float mag_y = event_mag.magnetic.y;
#endif
#if MZ
  float mag_z = event_mag.magnetic.z;
#endif
#endif

  /* Tratamento de Dados */
#if BARO

  float altura = filtros(altitudeReal, 1);
  int queda = detector_queda(altura);

  // Paraquedas
  paraquedas1(altura, queda);
  paraquedas2(altura, queda);
  paraquedas3(altura, queda);
  paraquedas4(altura, queda);

#endif

  /* Imprimindo Dados */

  String dataString = "";

#if BARO
  dataString += String(tempo) + "\t";
#if TERMOMETRO
  dataString += String(temperatura) + "\t";
#endif
#if PRESSAO
  dataString += String(pressao) + "\t";
#endif
  dataString += String(altura) + "\t";
  dataString += String(altitudeReal) + "\t";
  dataString += String(queda) + "\t";
  dataString += String(paraquedas_1) + "\t";
  dataString += String(paraquedas_2) + "\t";
  dataString += String(paraquedas_3) + "\t";
  dataString += String(paraquedas_4) + "\t";
#endif

#if ACEL
#if AX
  dataString += String(accel_x) + "\t";
#endif
#if AY
  dataString += String(accel_y) + "\t";
#endif
#if AZ
  dataString += String(accel_z) + "\t";
#endif
#endif

#if GIRO
#if GX
  dataString += String(gyro_x) + "\t";
#endif
#if GY
  dataString += String(gyro_y) + "\t";
#endif
#if GZ
  dataString += String(gyro_z) + "\t";
#endif
#endif

#if MAG
#if MX
  dataString += String(mag_x) + "\t";
#endif
#if MY
  dataString += String(mag_y) + "\t";
#endif
#if MZ
  dataString += String(mag_z) + "\t";
#endif
#endif

#if USANDO_GPS
#if GPS_LAT
  dataString += String(lat, 6) + "\t";
#endif
#if GPS_LNG
  dataString += String(lng, 6);
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
  unsigned long transmissao_lora = 0;
  if (timer_lora - transmissao_lora >= 3000) {
    transmissao_lora = timer_lora;
    LoRa.println(dataString);
  }
#endif
}