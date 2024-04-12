#define SENSORES 1

#define GYRO (SENSORES && 0)
#define GX (GYRO && 1)
#define GY (GYRO && 1)
#define GZ (GYRO && 1)

#define MAG (SENSORES && 0)
#define MX (MAG && 1)
#define MY (MAG && 0)
#define MZ (MAG && 0)

#define ACEL (SENSORES && 0)
#define AX (ACEL && 1)
#define AY (ACEL && 0)
#define AZ (ACEL && 0)

#define BAR (SENSORES && 1)

#define PARAQUEDAS (BAR && 1)
#define P1 (PARAQUEDAS && 1)
#define P2 (PARAQUEDAS && 0)
#define P3 (PARAQUEDAS && 0)
#define P4 (PARAQUEDAS && 0)


#define SD_CARD 0

#include <SPI.h>
#include <Wire.h>

#if (SENSORES)
#include <Adafruit_Sensor.h>
#endif

#if (BAR)
#include <Adafruit_BMP085.h>
#endif

#if (ACEL)
#include <Adafruit_ADXL345_U.h>
#endif

#if (MAG)
#include <Adafruit_HMC5883_U.h>
#endif

#if (GYRO)
#include <L3G.h>
#endif

#if (SD_CARD)
#include <SD.h>
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
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
#endif

#if (MAG)
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(123456);
#endif

#if (BAR)
Adafruit_BMP085 bmp;
#endif

#if (GYRO)
L3G gyro;
#endif

//Definindo SD
#if (SD_CARD)
#define chipSelect 53
String nomeBaseSD = "data";
String nomeSD;
#endif

//Definindo variaveis filtros
#if (BAR)
float var;
float alturaInicial;
#define numLeituras 20
float leituras[numLeituras];
float medias[numLeituras];
int indiceLeitura = 0;
int indiceMedia = 0;
float soma = 0;
float somaMedias = 0;
float media = 0;
float mediaDasMedias = 0;
#endif

//Definindo variaveis apogeu
#if (BAR)
#define historicoTamanho 20
float historico[historicoTamanho];
int indiceHistorico = 0;
int contadorHistorico = 0;
#endif


//Definindo variaveis paraquedas
#define intervaloTempo 10000
#define intervaloDelay 5000

#if (P1)
bool paraquedas1 = false;
bool paraquedas1data = false;
unsigned long tempoP1 = 0;
#endif

#if (P2)
bool paraquedas2 = false;
bool paraquedas2data = false;
unsigned long tempoP2 = 0;
#endif

#if (P3)
bool paraquedas3 = false;
bool paraquedas3data = false;
unsigned long tempoP3 = 0;
#endif

#if (P4)
bool paraquedas4 = false;
bool paraquedas4data = false;
unsigned long tempoP4 = 0;
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

#if (SD_CARD)
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }
#endif

#if (ACEL)
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_16_G);
#endif

#if (GYRO)
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }
  gyro.enableDefault();
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
  for (int i = 0; i < numLeituras; i++) {
    soma += bmp.readAltitude();
  }

  alturaInicial = soma / numLeituras;
  soma = 0;

  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - alturaInicial;
    soma += leituras[i];
    medias[i] = 0;
  }

  //Inicializando vetor de historico
  for (int i = 0; i < historicoTamanho; i++) {
    historico[i] = 0;
  }
#endif

  //Definindo cabecalho
  String dataStringInicial = "";
  dataStringInicial += "Tempo(s)\t";
#if (BAR)
  dataStringInicial += "Temperature(*C)\t";
  dataStringInicial += "Pressure(Pa)\t";
  dataStringInicial += "Altitude com primeiro filtro(m)\t";
  dataStringInicial += "Altitude com segundo filtro(m)\t";
  dataStringInicial += "Altitude sem filtro(m)\t";
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
#if (SD)
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

  unsigned long currentTime = millis();

#if (ACEL)
  sensors_event_t eventACEL;
  accel.getEvent(&eventACEL);
#endif
#if (AX)
  acelX = eventACEL.acceleration.x;
#endif
#if (AY)
  acelY = eventACEL.acceleration.y;
#endif
#if (AZ)
  acelZ = eventACEL.acceleration.z;
#endif

#if (GYRO)
  gyro.read();
#endif
#if (GX)
  gyroX = gyro.g.x;
#endif
#if (GY)
  gyroY = gyro.g.y;
#endif
#if (GZ)
  gyroZ = gyro.g.z;
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

//Filtro 1
#if (BAR)
  soma -= leituras[indiceLeitura];
  var = bmp.readAltitude() - alturaInicial;
  leituras[indiceLeitura] = var;
  soma += leituras[indiceLeitura];
  if (++indiceLeitura >= numLeituras) {
    indiceLeitura = 0;
  }

  media = soma / numLeituras;
#endif

//Filtro 2
#if (BAR)
  somaMedias -= medias[indiceMedia];
  medias[indiceMedia] = media;
  somaMedias += medias[indiceMedia];
  if (++indiceMedia >= numLeituras) {
    indiceMedia = 0;
  }

  mediaDasMedias = somaMedias / numLeituras;
#endif

//Apogeu
#if (BAR)
  historico[indiceHistorico] = mediaDasMedias;
  if (++indiceHistorico >= historicoTamanho) {
    indiceHistorico = 0;
  }

  for (int i = 1; i < historicoTamanho; i++) {
    if (historico[(indiceHistorico + i - 1) % historicoTamanho] > historico[(indiceHistorico + i) % historicoTamanho]) {
      contadorHistorico++;
    }
  }

  bool estaDescendo = false;

  if (contadorHistorico >= 0.7 * historicoTamanho) {
    estaDescendo = true;
  }
  contadorHistorico = 0;
#endif

  //Paraquedas 1
#if (P1)
  if (estaDescendo && !paraquedas1) {
    paraquedas1 = true;
    tempoP1 = millis();
    paraquedas1data = true;
    digitalWrite(IGN_1, HIGH);
  }
  if (paraquedas1 && currentTime >= tempoP1 + intervaloTempo) {
    paraquedas1data = false;
    digitalWrite(IGN_1, LOW);
  }
#endif

  //Paraquedas 2
#if (P2)
  if (estaDescendo && !paraquedas2) {
    paraquedas2 = true;
    tempoP2 = millis();
  }
  if (paraquedas2 && currentTime >= tempoP2 + intervaloDelay && currentTime < tempoP2 + intervaloDelay + intervaloTempo) {
    paraquedas2data = true;
    digitalWrite(IGN_2, HIGH);
  } else if (paraquedas2 && currentTime >= tempoP2 + intervaloDelay + intervaloTempo) {
    paraquedas2data = false;
    digitalWrite(IGN_2, LOW);
  }
#endif

//Paraquedas 3
#if (P3)
  if (estaDescendo && !paraquedas3) {
    paraquedas3 = true;
  }
  if (paraquedas3 && mediaDasMedias <= -3 && tempoP3 == 0) {
    tempoP3 = millis();
    paraquedas3data = true;
    digitalWrite(IGN_3, HIGH);
  }
  if (paraquedas3 && tempoP3 != 0 && currentTime >= tempoP3 + intervaloTempo) {
    paraquedas3data = false;
    digitalWrite(IGN_3, LOW);
  }
#endif

//Paraquedas 4
#if (P4)
  if (estaDescendo && !paraquedas4) {
    paraquedas4 = true;
  }
  if (paraquedas4 && mediaDasMedias <= -3 && tempoP4 == 0) {
    tempoP4 = millis();
  }
  if (paraquedas4 && tempoP4 != 0 && currentTime >= tempoP4 + intervaloDelay && currentTime < tempoP4 + intervaloDelay + intervaloTempo) {
    paraquedas4data = true;
    digitalWrite(IGN_4, HIGH);
  } else if (paraquedas4 && currentTime >= tempoP4 + intervaloDelay + intervaloTempo) {
    paraquedas4data = false;
    digitalWrite(IGN_4, LOW);
  }
#endif

  //Inicializando a string
  String dataString = "";
  //String de dados
  dataString += String(currentTime / 1000.0) + "\t";

#if (BAR)
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(media) + "\t";
  dataString += String(mediaDasMedias) + "\t";
  dataString += String(var) + "\t";
  dataString += String(estaDescendo) + "\t";
#endif

#if (P1)
  dataString += String(paraquedas1data) + "\t";
#endif

#if (P2)
  dataString += String(paraquedas2data) + "\t";
#endif

#if (P3)
  dataString += String(paraquedas3data) + "\t";
#endif

#if (P4)
  dataString += String(paraquedas4data) + "\t";
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
