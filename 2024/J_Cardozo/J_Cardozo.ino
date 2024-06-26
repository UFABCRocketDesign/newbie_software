//---------------------------------------------------------DEFINICOES---------------------------------------------------------

#define SENSORES 1

#define GYRO (SENSORES && 1)
#define GX (GYRO && 1)
#define GY (GYRO && 1)
#define GZ (GYRO && 1)

#define MAG (SENSORES && 1)
#define MX (MAG && 1)
#define MY (MAG && 1)
#define MZ (MAG && 1)

#define ACEL (SENSORES && 1)
#define AX (ACEL && 1)
#define AY (ACEL && 1)
#define AZ (ACEL && 1)

#define BAR (SENSORES && 1)

#define PARAQUEDAS (BAR && 1)
#define SAUDE (PARAQUEDAS && 1)
#define P1 (PARAQUEDAS && 1)
#define P2 (PARAQUEDAS && 1)
#define P3 (PARAQUEDAS && 1)
#define P4 (PARAQUEDAS && 1)

#define SD_CARD 0

#define RFREQ 0 //Modulo de radio frequencia tradicional

#define GPS 0

#define LORA 0

#define SERIAL_PRINT 1

#define WUF 1 //Wait until flight

#define BUZZER 1


//---------------------------------------------------------INICIALIZACOES---------------------------------------------------------
#if ((GYRO) || (MAG) || (ACEL) || (BAR))
#include <Wire.h>
#endif

#if (BAR)
#include "src/lib/Barometro/BMP085.h"
#include "src/lib/Filtros/Filtros.h"
#include "src/lib/Apogeu/Apogeu.h"

BMP085 bmp;

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

#if (ACEL)
//#include <Adafruit_ADXL345_U.h>
//Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
#include "src/lib/Acelerometro/ADXL345.h"
ADXL345 acel(2);
#endif

#if (MAG)
//#include <Adafruit_HMC5883_U.h>
//Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(123456);
#include "src/lib/Magnetometro/HMC5883L.h"
HMC5883L mag;
#endif

#if (GYRO)
//#include <L3G.h>
//L3G gyro;

#include "src/lib/Giroscopio/L3G4200D.h"
L3G4200D gyro(100, 250);
#endif

#if (SD_CARD)
#include <SD.h>
#include <SPI.h>

//Definindo SD
#define chipSelect 53
String nomeBaseSD = "joao";  //setup (talvez um define?)
String nomeSD;               //global
#endif

#if (PARAQUEDAS)
#include "src/lib/Paraquedas/Paraquedas.h"

#if (P1)
#define IGN_1 36 /*act1*/
#define HEAL_1 68	/*health 1*/
#endif

#if (P2)
#define IGN_2 61 /*act2*/
#define HEAL_2 62	/*health 2*/
#endif

#if (P3)
#define IGN_3 46 /*act3*/
#define HEAL_3 56	/*health 3*/
#endif

#if (P4)
#define IGN_4 55 /*act4*/
#define HEAL_4 58	/*health 4*/
#endif

//Inicializando paraquedas
Paraquedas p1(10000, 0, IGN_1, HEAL_1, 0);
Paraquedas p2(10000, 5000, IGN_2, HEAL_2, 0);
Paraquedas p3(10000, 0, IGN_3, HEAL_3, -3);
Paraquedas p4(10000, 5000, IGN_4, HEAL_4, -3);

#endif

#if (GPS)
#include <TinyGPSPlus.h>
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
HardwareSerial &GPSSerial = Serial1;
#endif

#if (LORA) //Comunicacao por modulo LoRa
#define LoRaDelay 3000
#define LoRa LongRange
HardwareSerial &LoRa(Serial3);
unsigned long previousMillisLora = 0;
#endif

#if (RFREQ) //Comunicacao por radio frequencia
#include <RH_ASK.h>
RH_ASK rf_driver;
unsigned long previousMillisRFREQ = 0;
const long interval = 200;
#endif

#if (WUF) //Wait Until Flight
bool wuf = true;
const float wufAltura = 50;
#endif

#if (BUZZER)
#include "src/lib/Buzzer/buzzer.h"
#define BUZZ_PIN A0
Buzzer BeepSistemas(BUZZ_PIN, 5000, 100);
#endif

//Inicializando variavel de tempo
unsigned long currentTime = 0;
//Inicializando a string
String dataString = "";
//Inicializando altitude
float altitude = 0;


//---------------------------------------------------------FUNCOES---------------------------------------------------------


void readAll() {
bool booleano;
//Definindo variaveis acelerometro, giroscopio e magnetometro
#if (ACEL)
  booleano = acel.lerTudo();
#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif

#if (GYRO)
  booleano = gyro.lerTudo();
#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif

#if (MAG)
  booleano = mag.lerTudo();
#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif

#if (BAR)
  //Filtros
  booleano = bmp.lerTudo();
  altitude = bmp.getAltitude() - alturaInicial;

  f1.aplicarFiltro(altitude);
  f2.aplicarFiltro(f1.getMedia());
  f3.aplicarFiltro(f2.getMedia());

#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif

#if (PARAQUEDAS)

#if (P1)
  booleano = p1.getInfo();
#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif

#if (P2)
  booleano = p2.getInfo();
#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif

#if (P3)
  booleano = p3.getInfo();
#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif

#if (P4)
  booleano = p4.getInfo();
#if (BUZZER)
    BeepSistemas.addSystem(booleano);
#endif
#endif
#endif

#if (GPS)
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }
#endif

  //String de dados
  dataString = "";
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
#if (WUF)
  dataString += String(wuf) + "\t";
#endif

#if (PARAQUEDAS)
#if (P1)
  dataString += String(p1.getData());
#if (SAUDE)
  dataString += String(p1.getInfo());
#endif
#endif

#if (P2)
  dataString += String(p2.getData());
#if (SAUDE)
  dataString += String(p2.getInfo());
#endif
#endif

#if (P3)
  dataString += String(p3.getData());
#if (SAUDE)
  dataString += String(p3.getInfo());
#endif
#endif

#if (P4)
  dataString += String(p4.getData());
#if (SAUDE)
  dataString += String(p4.getInfo());
#endif
#endif
#endif

#if (AX)
  dataString += String(acel.getX()) + "\t";
#endif
#if (AY)
  dataString += String(acel.getY()) + "\t";
#endif
#if (AZ)
  dataString += String(acel.getZ()) + "\t";
#endif

#if (GX)
  dataString += String(gyro.getX()) + "\t";
#endif
#if (GY)
  dataString += String(gyro.getY()) + "\t";
#endif
#if (GZ)
  dataString += String(gyro.getZ()) + "\t";
#endif

#if (MX)
  dataString += String(mag.getX()) + "\t";
#endif
#if (MY)
  dataString += String(mag.getY()) + "\t";
#endif
#if (MZ)
  dataString += String(mag.getZ()) + "\t";
#endif
#if (GPS)
  dataString += String(gps.location.lat(), 6) + "\t";
  dataString += String(gps.location.lng(), 6) + "\t";
  dataString += String(gps.satellites.value()) + "\t";
  dataString += String(gps.hdop.hdop()) + "\t";
  dataString += String(gps.location.age()) + "\t";
  dataString += String(gps.altitude.meters()) + "\t";
  dataString += String(gps.speed.mph()) + "\t";
#endif
}

void doAll() {
#if (BAR)
  // Obtendo a média final filtrada
  float filtroFinal = f3.getMedia();

  //Apogeu
  apogeu.deteccaoApogeu(filtroFinal, 0);  //0 para deteccao de apogeu comum || 1 para deteccao de apogeu personalizada
#endif

  //Paraquedas 1
#if (P1)
  p1.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo(), bmp.getEstaDescendoTimer());
#endif

  //Paraquedas 2
#if (P2)
  p2.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo(), bmp.getEstaDescendoTimer());
#endif

//Paraquedas 3
#if (P3)
  p3.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo(), bmp.getEstaDescendoTimer());
#endif

//Paraquedas 4
#if (P4)
  p4.ativarParaquedas(filtroFinal, currentTime, apogeu.getEstaDescendo(), bmp.getEstaDescendoTimer());
#endif
}

void writeAll() {
  
#if (SERIAL_PRINT)
  Serial.println(dataString);
#endif

#if (LORA)
  if (currentTime - previousMillisLora >= LoRaDelay) {
    previousMillisLora = currentTime;
    LoRa.println(dataString);
  }
#endif

#if (RFREQ)


  if (currentTime - previousMillisRFREQ >= interval) {
    char msg[64];
    dataString.toCharArray(msg, 64);
    // salva o tempo atual como o último tempo de execução
    previousMillisRFREQ = currentTime;

    // Inverte o estado do LED
    int ledState = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !ledState);

    // Envia os dados
    if (rf_driver.send((uint8_t *)msg, strlen(msg))) {
      rf_driver.waitPacketSent();
    } else {
      Serial.print("\tFailed to send message.");
    }
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

//---------------------------------------------------------SETUP---------------------------------------------------------

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

#if (LORA)
  LoRa.begin(9600);
#endif

#if (P1)
  p1.begin();
#endif

#if (P2)
  p2.begin();
#endif

#if (P3)
  p3.begin();
#endif

#if (P4)
  p4.begin();
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

#if (GPS)
  GPSSerial.begin(GPSBaud);
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

#if (WUF)
  dataStringInicial += "WUF\t";
#endif

#if (P1)
  dataStringInicial += "Paraquedas 1\t";
#if (SAUDE)
  dataStringInicial += "SaudeParaquedas 1 \t";
#endif
#endif

#if (P2)
  dataStringInicial += "Paraquedas 2\t";
#if (SAUDE)
  dataStringInicial += "SaudeParaquedas 2 \t";
#endif
#endif

#if (P3)
  dataStringInicial += "Paraquedas 3\t";
#if (SAUDE)
  dataStringInicial += "SaudeParaquedas 3 \t";
#endif
#endif

#if (P4)
  dataStringInicial += "Paraquedas 4\t";
#if (SAUDE)
  dataStringInicial += "SaudeParaquedas 4 \t";
#endif
#endif

#if (AX)
  dataStringInicial += "Acel X(G)\t";
#endif

#if (AY)
  dataStringInicial += "Acel Y(G)\t";
#endif

#if (AZ)
  dataStringInicial += "Acel Z(G)\t";
#endif

#if (GX)
  dataStringInicial += "Gyro X(deg/s)\t";
#endif

#if (GY)
  dataStringInicial += "Gyro Y(deg/s)\t";
#endif

#if (GZ)
  dataStringInicial += "Gyro Z(deg/s)\t";
#endif

#if (MX)
  dataStringInicial += "Mag X(uT)\t";
#endif

#if (MY)
  dataStringInicial += "Mag Y(uT)\t";
#endif

#if (MZ)
  dataStringInicial += "Mag Z(uT)\t";
#endif

#if (GPS)
  dataStringInicial += "Latitude (deg)\t";
  dataStringInicial += "Longitude (deg)\t";
  dataStringInicial += "Satelites \t";
  dataStringInicial += "Precisao \t";
  dataStringInicial += "Age \t";
  dataStringInicial += "AltitudeGPS (m)\t";
  dataStringInicial += "VelocidadeGPS (km/h)\t";

#endif

#if (SERIAL_PRINT)
  Serial.println(dataStringInicial);
#endif

#if (LORA)
  LoRa.println(dataStringInicial);
#endif

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

    //Serial.println("Verificando: " + nomeSD);

    if (SD.exists(nomeSD)) {
      //Serial.println(nomeSD + " já existe.");
    } else {
      //Serial.println(nomeSD + " não existe, criando...");
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
       

#if (WUF)
  while((wufAltura >= altitude) && (altitude >= -1*wufAltura)) {
      currentTime = millis();
      readAll();
#if (BUZZER)
      BeepSistemas.addSystem(true);
      BeepSistemas.beep(currentTime, false);
#endif
      writeAll();
  }
  wuf = false;
#endif
}

//---------------------------------------------------------LOOP---------------------------------------------------------


void loop() {

  currentTime = millis();
  readAll();

  doAll();

  writeAll();
}