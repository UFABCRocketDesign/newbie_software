#define SENSORES 1

#define GYRO (SENSORES && 1)
#define GX (GYRO && 1)
#define GY (GYRO && 1)
#define GZ (GYRO && 1)

#define MAG (SENSORES && 0)
#define MX (MAG && 1)
#define MY (MAG && 1)
#define MZ (MAG && 1)

#define ACEL (SENSORES && 1)
#define AX (ACEL && 1)
#define AY (ACEL && 1)
#define AZ (ACEL && 1)

#define BAR (SENSORES && 1)

#define PARAQUEDAS (BAR && 1)
#define P1 (PARAQUEDAS && 1)
#define P2 (PARAQUEDAS && 1)
#define P3 (PARAQUEDAS && 1)
#define P4 (PARAQUEDAS && 1)

#define SD_CARD 1

#define RFREQ 0

#include <SPI.h>
#include <Wire.h>

#if (SENSORES)
#include <Adafruit_Sensor.h>
#endif

#if (BAR)
#include "src/lib/Barometro/BMP085.h"
#include "src/lib/Filtros/Filtros.h"
#include "src/lib/Apogeu/Apogeu.h"
#endif

#if (ACEL)
//#include <Adafruit_ADXL345_U.h>
#include "src/lib/Acelerometro/ADXL345.h"
#endif

#if (MAG)
#include <Adafruit_HMC5883_U.h>
#endif

#if (GYRO)
//#include <L3G.h>
#include "src/lib/Giroscopio/L3G4200D.h"
#endif

#if (SD_CARD)
#include <SD.h>
#endif

#if (PARAQUEDAS)
#include "src/lib/Paraquedas/Paraquedas.h"
#endif

#if (P1)
#define IGN_1 36 /*act1*/
#endif

#if (P2)
#define IGN_2 61 /*act2*/
#endif

#if (P3)
#define IGN_3 46 /*act3*/
#endif

#if (P4)
#define IGN_4 55 /*act4*/
#endif

#if (ACEL)
//Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
ADXL345 acel(2);
#endif

#if (MAG)
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(123456);
#endif

#if (BAR)
BMP085 bmp;
#endif

#if (GYRO)
//L3G gyro;
L3G4200D gyro(100, 250);
#endif

#if (RFREQ)
#include <RH_ASK.h>
RH_ASK rf_driver;
unsigned long previousMillis = 0;
const long interval = 200;
#endif

//Definindo SD
#if (SD_CARD)
#define chipSelect 53
String nomeBaseSD = "joao";  //setup
String nomeSD;               //global
#endif

#if (BAR)
//Definindo variaveis para os filtros
#define NUM_LEITURAS 10
#define NUM_FILTROS 3

//Inicializando os filtros
FiltroMediaMovel f1(NUM_LEITURAS);
FiltroMediaMovel f2(NUM_LEITURAS);
FiltroMediaMovel f3(NUM_LEITURAS);

//Inicializando apogeu
Apogeu apogeu(20, 0.85f);

float alturaInicial;  //Variavel global para altura inicial

#endif

#if (PARAQUEDAS)

//Inicializando paraquedas
Paraquedas p1(10000, 0, IGN_1, 0);
Paraquedas p2(10000, 5000, IGN_2, 0);
Paraquedas p3(10000, 0, IGN_3, -3);
Paraquedas p4(10000, 5000, IGN_4, -3);

#endif

void setup() {
  Serial.begin(115200);
  Wire.begin();

//Inicializando BMP, SD, Acelerometro, Giroscopio e Magnetometro
#if (BAR)
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif

#if (RFREQ)
  if (!rf_driver.init()) {
    Serial.println("RF init failed");
  } else {
    Serial.println("RF init succeeded");
  }
  pinMode(LED_BUILTIN, OUTPUT);
#endif

#if (SD_CARD)
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }
#endif

#if (ACEL)
  // if (!accel.begin()) {
  //   Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  //   while (1)
  //     ;
  // }
  // accel.setRange(ADXL345_RANGE_16_G);
  if (!acel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif

#if (GYRO)
  // if (!gyro.init()) {
  //   Serial.println("Failed to autodetect gyro type!");
  //   while (1)
  //     ;
  // }
  // gyro.enableDefault();
  if (!gyro.begin()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }
#endif


#if (MAG)
  if (!mag.begin()) {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif

//Definindo pinos dos paraquedas
#if (P1)
  pinMode(IGN_1, OUTPUT);
#endif

#if (P2)
  pinMode(IGN_2, OUTPUT);
#endif

#if (P3)
  pinMode(IGN_3, OUTPUT);
#endif

#if (P4)
  pinMode(IGN_4, OUTPUT);
#endif

#if (BAR)
  //Primeiras leituras BMP
  float soma = 0;
  for (int i = 0; i < NUM_LEITURAS; i++) {
    bmp.lerTudo();
    soma += bmp.getAltitude();
  }
  alturaInicial = soma / NUM_LEITURAS;

#endif

  //Definindo cabecalho
  String dataStringInicial = "";
  dataStringInicial += "Tempo(s)\t";
#if (BAR)
  dataStringInicial += "Temperature(*C)\t";
  dataStringInicial += "Pressure(Pa)\t";
  dataStringInicial += "Altitude sem filtro(m)\t";
  for (int i = 1; i <= NUM_FILTROS; i++) {
    dataStringInicial += "Altitude com filtro";
    dataStringInicial += String(i);
    dataStringInicial += "(m)\t";
  }
  dataStringInicial += "Status\t";
#endif

#if (P1)
  dataStringInicial += "Paraquedas 1\t";
#endif

#if (P2)
  dataStringInicial += "Paraquedas 2\t";
#endif

#if (P3)
  dataStringInicial += "Paraquedas 3\t";
#endif

#if (P4)
  dataStringInicial += "Paraquedas 4\t";
#endif

#if (AX)
  dataStringInicial += "Acel X\t";
#endif

#if (AY)
  dataStringInicial += "Acel Y\t";
#endif

#if (AZ)
  dataStringInicial += "Acel Z\t";
#endif

#if (GX)
  dataStringInicial += "Gyro X\t";
#endif

#if (GY)
  dataStringInicial += "Gyro Y\t";
#endif

#if (GZ)
  dataStringInicial += "Gyro Z\t";
#endif

#if (MX)
  dataStringInicial += "Mag X\t";
#endif

#if (MY)
  dataStringInicial += "Mag Y\t";
#endif

#if (MZ)
  dataStringInicial += "Mag Z\n";
#endif

  Serial.println(dataStringInicial);


//Logica para nome do arquivo SD
#if (SD_CARD)
  int iSD = 0;
  while (true) {
    int numZeros = 8 - nomeBaseSD.length() - String(iSD).length();
    String zeros = "";
    for (int i = 0; i < numZeros; i++) {
      zeros += "0";
    }

    nomeSD = nomeBaseSD + zeros + String(iSD) + ".txt";

    Serial.println("Verificando: " + nomeSD);

    if (SD.exists(nomeSD)) {
      Serial.println(nomeSD + " já existe.");
    } else {
      Serial.println(nomeSD + " não existe, criando...");
      File dataFile = SD.open(nomeSD, FILE_WRITE);
      if (dataFile) {
        Serial.println(nomeSD + " criado.");
        dataFile.println(dataStringInicial);
        dataFile.close();
      } else {
        Serial.println("Erro ao abrir " + nomeSD);
      }
      break;
    }
    iSD++;
  }
#endif
}

void loop() {

  unsigned long currentTime = millis();

//Definindo variaveis acelerometro, giroscopio e magnetometro
#if (AX)
  float acelX;
#endif
#if (AY)
  float acelY;
#endif
#if (AZ)
  float acelZ;
#endif

#if (GX)
  float gyroX;
#endif
#if (GY)
  float gyroY;
#endif
#if (GZ)
  float gyroZ;
#endif

#if (MX)
  float magX;
#endif
#if (MY)
  float magY;
#endif
#if (MZ)
  float magZ;
#endif

#if (ACEL)
  // sensors_event_t eventACEL;
  // accel.getEvent(&eventACEL);
  acel.lerTudo();
#endif
// #if (AX)
//   acelX = eventACEL.acceleration.x;
// #endif
// #if (AY)
//   acelY = eventACEL.acceleration.y;
// #endif
// #if (AZ)
//   acelZ = eventACEL.acceleration.z;
// #endif

#if (AX)
  acelX = acel.getX();
#endif
#if (AY)
  acelY = acel.getY();
#endif
#if (AZ)
  acelZ = acel.getZ();
#endif

#if (GYRO)
  //gyro.read();
  gyro.lerTudo();
#endif
#if (GX)
  gyroX = gyro.getX();
#endif
#if (GY)
  gyroY = gyro.getY();
#endif
#if (GZ)
  gyroZ = gyro.getZ();
#endif

#if (MAG)
  sensors_event_t eventMAG;
  mag.getEvent(&eventMAG);
#endif
#if (MX)
  magX = eventMAG.magnetic.x;
#endif
#if (MY)
  magY = eventMAG.magnetic.y;
#endif
#if (MZ)
  magZ = eventMAG.magnetic.z;
#endif

#if (BAR)
  //Filtros
  bmp.lerTudo();
  float altitude = bmp.getAltitude() - alturaInicial;

  f1.aplicarFiltro(altitude);
  f2.aplicarFiltro(f1.getMedia());
  f3.aplicarFiltro(f2.getMedia());

  // Obtendo a média final filtrada
  float filtroFinal = f3.getMedia();

  //Apogeu
  apogeu.deteccaoApogeu(filtroFinal);
#endif

  //Paraquedas 1
#if (P1)
  p1.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo());
#endif

  //Paraquedas 2
#if (P2)
  p2.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo());
#endif

//Paraquedas 3
#if (P3)
  p3.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo());
#endif

//Paraquedas 4
#if (P4)
  p4.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo());
#endif

  //Inicializando a string
  String dataString = "";
  //String de dados
  dataString += String(currentTime / 1000.0) + "\t";

#if (BAR)
  dataString += String(bmp.getTemperatura()) + "\t";
  dataString += String(bmp.getPressao()) + "\t";
  dataString += String(altitude) + "\t";
  dataString += String(f1.getMedia()) + "\t";
  dataString += String(f2.getMedia()) + "\t";
  dataString += String(f3.getMedia()) + "\t";
  dataString += String(apogeu.getEstaDescendo()) + "\t";
#endif

#if (P1)
  dataString += String(p1.getData()) + "\t";
#endif

#if (P2)
  dataString += String(p2.getData()) + "\t";
#endif

#if (P3)
  dataString += String(p3.getData()) + "\t";
#endif

#if (P4)
  dataString += String(p4.getData()) + "\t";
#endif

#if (AX)
  dataString += String(acelX) + "\t";
#endif
#if (AY)
  dataString += String(acelY) + "\t";
#endif
#if (AZ)
  dataString += String(acelZ) + "\t";
#endif

#if (GX)
  dataString += String(gyroX) + "\t";
#endif
#if (GY)
  dataString += String(gyroY) + "\t";
#endif
#if (GZ)
  dataString += String(gyroZ) + "\t";
#endif

#if (MX)
  dataString += String(magX) + "\t";
#endif
#if (MY)
  dataString += String(magY) + "\t";
#endif
#if (MZ)
  dataString += String(magZ) + "\t";
#endif

  Serial.println(dataString);

#if (RFREQ)
  char msg[64];
  dataString.toCharArray(msg, 64);

  if (currentTime - previousMillis >= interval) {
    // salva o tempo atual como o último tempo de execução
    previousMillis = currentTime;

    // Inverte o estado do LED
    int ledState = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !ledState);
  }

  // Envia os dados
  if (rf_driver.send((uint8_t*)msg, strlen(msg))) {
    rf_driver.waitPacketSent();
  } else {
    Serial.print("\tFailed to send message.");
  }

  // Desliga o LED
  digitalWrite(LED_BUILTIN, LOW);
#endif

#if (SD_CARD)
  File dataFile = SD.open(nomeSD, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.print("Error opening ");
    Serial.print(nomeSD);
    Serial.println();
  }
#endif
}