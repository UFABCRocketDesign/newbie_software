#define SENSORES 1

#define GYRO (SENSORES && 0)
#define GX (GYRO && 1)
#define GY (GYRO && 1)
#define GZ (GYRO && 1)

#define MAG (SENSORES && 0)
#define MX (MAG && 1)
#define MY (MAG && 1)
#define MZ (MAG && 1)

#define ACEL (SENSORES && 0)
#define AX (ACEL && 1)
#define AY (ACEL && 1)
#define AZ (ACEL && 1)

#define BAR (SENSORES && 1)

#define PARAQUEDAS (BAR && 1)
#define P1 (PARAQUEDAS && 1)
#define P2 (PARAQUEDAS && 1)
#define P3 (PARAQUEDAS && 1)
#define P4 (PARAQUEDAS && 1)

#define SD_CARD 0

#define RFREQ 0

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

#if (RFREQ)
#include <RH_ASK.h>
RH_ASK rf_driver;
unsigned long previousMillis = 0;
const long interval = 200;
#endif

//Definindo SD
#if (SD_CARD)
#define chipSelect 53
String nomeBaseSD = "data";  //setup
String nomeSD;               //global
#endif

//Definindo variaveis filtros
#if (BAR)
float alturaInicial;  //global
#define NUM_FILTROS 3
#define NUM_LEITURAS 10
float dados[NUM_FILTROS][NUM_LEITURAS];
int indices[NUM_FILTROS];
float filtros[NUM_FILTROS];
#endif

//Definindo variaveis apogeu
#if (BAR)
#define historicoTamanho 20
float historico[historicoTamanho];  //global
int indiceHistorico = 0;            //global
#endif

//Definindo variaveis paraquedas

#if (PARAQUEDAS)
#define intervaloTempo 10000  //Intervalo de tempo que os skibs ficam ligados
// #define NUM_PARAQUEDAS 4
bool paraquedas[] = { false, false, false, false };
bool paraquedasData[] = { false, false, false, false };
unsigned long paraquedasTempo[] = { 0, 0, 0, 0 };
const int ign[] = { IGN_1, IGN_2, IGN_3, IGN_4 };
// for (int i = 0; i < NUM_PARAQUEDAS; i++) {
//   paraquedas[i] = false;
//   paraquedasData[i] = false;
//   paraquedasTempo[i] = 0;
// }
#endif

#if (BAR)
float aplicarFiltro(float entrada, int filtro) {
  float soma = 0;
  float media = 0;
  dados[filtro][indices[filtro]] = entrada;
  for (int j = 0; j < NUM_LEITURAS; j++) {
    soma += dados[filtro][j];
  }
  media = soma / NUM_LEITURAS;
  indices[filtro]++;
  if (indices[filtro] >= NUM_LEITURAS) {
    indices[filtro] = 0;
  }
  return media;
}

bool deteccaoApogeu(float entrada) {
  historico[indiceHistorico] = entrada;
  int contadorHistorico = 0;
  if (++indiceHistorico >= historicoTamanho) {
    indiceHistorico = 0;
  }

  for (int i = 1; i < historicoTamanho; i++) {
    if (historico[(indiceHistorico + i - 1) % historicoTamanho] > historico[(indiceHistorico + i) % historicoTamanho]) {
      contadorHistorico++;
    }
  }

  bool estaDescendo = false;

  if (contadorHistorico >= 0.85 * historicoTamanho) {
    estaDescendo = true;
  }
  return estaDescendo;
}
#endif

#if (PARAQUEDAS)
void ativarParaquedas(unsigned long currentTime, bool estaDescendo, float delay, float altura, int indiceParaquedas) {

  if (delay == 0) {
    if (altura == 0) {
      if (estaDescendo && !paraquedas[indiceParaquedas]) {
        paraquedas[indiceParaquedas] = true;
        paraquedasTempo[indiceParaquedas] = millis();
        paraquedasData[indiceParaquedas] = true;
        digitalWrite(ign[indiceParaquedas], HIGH);
      }

      if (paraquedas[indiceParaquedas] && currentTime >= paraquedasTempo[indiceParaquedas] + intervaloTempo) {
        paraquedasData[indiceParaquedas] = false;
        digitalWrite(ign[indiceParaquedas], LOW);
      }
    } else {
      if (estaDescendo && !paraquedas[indiceParaquedas]) {
        paraquedas[indiceParaquedas] = true;
      }
      if (paraquedas[indiceParaquedas] && filtros[NUM_FILTROS - 1] <= altura && paraquedasTempo[indiceParaquedas] == 0) {
        paraquedasTempo[indiceParaquedas] = millis();
        paraquedasData[indiceParaquedas] = true;
        digitalWrite(ign[indiceParaquedas], HIGH);
      }
      if (paraquedas[indiceParaquedas] && paraquedasTempo[indiceParaquedas] != 0 && currentTime >= paraquedasTempo[indiceParaquedas] + intervaloTempo) {
        paraquedasData[indiceParaquedas] = false;
        digitalWrite(ign[indiceParaquedas], LOW);
      }
    }
  } else {
    if (altura == 0) {
      if (estaDescendo && !paraquedas[indiceParaquedas]) {
        paraquedas[indiceParaquedas] = true;
        paraquedasTempo[indiceParaquedas] = millis();
      }
      if (paraquedas[indiceParaquedas] && currentTime >= paraquedasTempo[indiceParaquedas] + delay && currentTime < paraquedasTempo[indiceParaquedas] + delay + intervaloTempo) {
        paraquedasData[indiceParaquedas] = true;
        digitalWrite(ign[indiceParaquedas], HIGH);
      } else if (paraquedas[indiceParaquedas] && currentTime >= paraquedasTempo[indiceParaquedas] + delay + intervaloTempo) {
        paraquedasData[indiceParaquedas] = false;
        digitalWrite(ign[indiceParaquedas], LOW);
      }
    } else {
      if (estaDescendo && !paraquedas[indiceParaquedas]) {
        paraquedas[indiceParaquedas] = true;
      }
      if (paraquedas[indiceParaquedas] && filtros[NUM_FILTROS - 1] <= altura && paraquedasTempo[indiceParaquedas] == 0) {
        paraquedasTempo[indiceParaquedas] = millis();
      }
      if (paraquedas[indiceParaquedas] && paraquedasTempo[indiceParaquedas] != 0 && currentTime >= paraquedasTempo[indiceParaquedas] + delay && currentTime < paraquedasTempo[indiceParaquedas] + delay + intervaloTempo) {
        paraquedasData[indiceParaquedas] = true;
        digitalWrite(ign[indiceParaquedas], HIGH);
      } else if (paraquedas[indiceParaquedas] && currentTime >= paraquedasTempo[indiceParaquedas] + delay + intervaloTempo) {
        paraquedasData[indiceParaquedas] = false;
        digitalWrite(ign[indiceParaquedas], LOW);
      }
    }
  }
}
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
  float soma = 0;
  for (int i = 0; i < NUM_LEITURAS; i++) {
    soma += bmp.readAltitude();
  }
  alturaInicial = soma / NUM_LEITURAS;

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

#if (BAR)
  //Filtros
  float altitude = bmp.readAltitude() - alturaInicial;

  filtros[0] = aplicarFiltro(altitude, 0);
  for (int i = 0; i < NUM_FILTROS; i++) {
    filtros[i] = aplicarFiltro(filtros[i - 1], i);
  }

  //Apogeu
  bool estaDescendo = deteccaoApogeu(filtros[NUM_FILTROS - 1]);
#endif

  //Paraquedas 1
#if (P1)
  ativarParaquedas(currentTime, estaDescendo, 0, 0, 0);
#endif

  //Paraquedas 2
#if (P2)
  ativarParaquedas(currentTime, estaDescendo, 5000, 0, 1);
#endif

//Paraquedas 3
#if (P3)
  ativarParaquedas(currentTime, estaDescendo, 0, -3, 2);
#endif

//Paraquedas 4
#if (P4)
  ativarParaquedas(currentTime, estaDescendo, 5000, -3, 3);
#endif

  //Inicializando a string
  String dataString = "";
  //String de dados
  dataString += String(currentTime / 1000.0) + "\t";

#if (BAR)
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(altitude) + "\t";
  for (int i = 0; i < NUM_FILTROS; i++) {
    dataString += String(filtros[i]) + "\t";
  }
  dataString += String(estaDescendo) + "\t";
#endif

#if (P1)
  dataString += String(paraquedasData[0]) + "\t";
#endif

#if (P2)
  dataString += String(paraquedasData[1]) + "\t";
#endif

#if (P3)
  dataString += String(paraquedasData[2]) + "\t";
#endif

#if (P4)
  dataString += String(paraquedasData[3]) + "\t";
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
