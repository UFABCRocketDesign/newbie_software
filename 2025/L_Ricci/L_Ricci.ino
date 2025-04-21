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
#define SD_CARD 0
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

class FiltroMediaMovel {
private:
  float leituras[LEITURAS] = {};
  float total = 0;
  int indiceAtual = 0;
public:
  float filtro(float altitudeReal) {
    total = total - leituras[indiceAtual];
    leituras[indiceAtual] = (altitudeReal);
    total = total + leituras[indiceAtual];
    indiceAtual = (indiceAtual + 1) % LEITURAS;
    float altura = total / LEITURAS;

    return altura;
  }
  float getMedia() {
    return (total / LEITURAS);
  }
};

class Paraquedas {
private:
  int pinoIgnicao;
  int estadoParaquedas;
  unsigned long timer;
  unsigned long desativacao;
  bool usaDesativacao;
  bool ignicaoImediata;
public:
  Paraquedas(int pino, bool usaDesativacao, bool ignicaoImediata)
    : pinoIgnicao(pino), usaDesativacao(usaDesativacao), ignicaoImediata(ignicaoImediata) {}
  int ativar(float altura, int queda) {
#if BARO
    if (queda == 1 && estadoParaquedas == 0 && (ALTITUDE_TETO < 0 || altura < ALTITUDE_TETO)) {
      estadoParaquedas = 1;
      if (ignicaoImediata) {
        digitalWrite(pinoIgnicao, HIGH);
      }
      timer = millis();
    }

    if (estadoParaquedas == 1 && (millis() - timer) >= 2000) {
      estadoParaquedas = 2;
      if (!ignicaoImediata) {
        digitalWrite(pinoIgnicao, HIGH);
        desativacao = millis();
      } else {
        digitalWrite(pinoIgnicao, LOW);
      }
    }

    if (usaDesativacao && estadoParaquedas == 2 && (millis() - desativacao) >= 1500) {
      estadoParaquedas = 3;
      digitalWrite(pinoIgnicao, LOW);
    }
#endif
  }
#if BARO
  int getValor() {
    return estadoParaquedas;
  }
#endif
};

FiltroMediaMovel f1;
FiltroMediaMovel f2;

Paraquedas p1(IGN_1, false, true);
Paraquedas p2(IGN_2, true, false);
Paraquedas p3(IGN_3, false, true);
Paraquedas p4(IGN_4, true, false);

int queda = 0, contadorQueda = 0;
float altitudes[NUMERO_QUEDAS];
float alt = 0;

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

int detectorQueda(float altura) {
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

void loop() {
  unsigned long timerLora = millis();
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
  float accelX = event_accel.acceleration.x;
#endif
#if AY
  float accelY = event_accel.acceleration.y;
#endif
#if AZ
  float accelZ = event_accel.acceleration.z;
#endif
#endif

#if GIRO
  gyro.read();
#if GX
  int gyroX = gyro.g.x;
#endif
#if GY
  int gyroY = gyro.g.y;
#endif
#if GZ
  int gyroZ = gyro.g.z;
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
  float magX = event_mag.magnetic.x;
#endif
#if MY
  float magY = event_mag.magnetic.y;
#endif
#if MZ
  float magZ = event_mag.magnetic.z;
#endif
#endif

  /* Tratamento de Dados */
#if BARO

  float valorFiltrado1 = f1.filtro(altitudeReal);
  float valorFiltrado2 = f2.filtro(f1.getMedia());
  int queda = detectorQueda(valorFiltrado2);

  // Paraquedas
  p1.ativar(valorFiltrado2, queda);
  p2.ativar(valorFiltrado2, queda);
  p3.ativar(valorFiltrado2, queda);
  p4.ativar(valorFiltrado2, queda);

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
  dataString += String(valorFiltrado2) + "\t";
  dataString += String(altitudeReal) + "\t";
  dataString += String(queda) + "\t";
  dataString += String(p1.getValor()) + "\t";
  dataString += String(p2.getValor()) + "\t";
  dataString += String(p3.getValor()) + "\t";
  dataString += String(p4.getValor()) + "\t";
#endif

#if ACEL
#if AX
  dataString += String(accelX) + "\t";
#endif
#if AY
  dataString += String(accelY) + "\t";
#endif
#if AZ
  dataString += String(accelZ) + "\t";
#endif
#endif

#if GIRO
#if GX
  dataString += String(gyroX) + "\t";
#endif
#if GY
  dataString += String(gyroY) + "\t";
#endif
#if GZ
  dataString += String(gyroZ) + "\t";
#endif
#endif

#if MAG
#if MX
  dataString += String(magX) + "\t";
#endif
#if MY
  dataString += String(magY) + "\t";
#endif
#if MZ
  dataString += String(magZ) + "\t";
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
  unsigned long transmissaoLora = 0;
  if (timerLora - transmissaoLora >= 3000) {
    transmissaoLora = timerLora;
    LoRa.println(dataString);
  }
#endif
}