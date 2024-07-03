//#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <L3G.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_ADXL345_U.h>
#include "./src/lib/Apogeu/Apogeu.h"
#include "./src/lib/Filtro/Filtro.h"
#include "./src/lib/Paraquedas/Paraquedas.h"
#include "./src/lib/Barometro/Barometro.h"

//SDCard
#define SDCARD (0)
//Paraquedas
#define PARA (1)
#define IGN_1 36  //Paraquedas 1
#define IGN_2 61  //Paraquedas 2
#define IGN_3 46  //Paraquedas 3
#define IGN_4 55  //Paraquedas 4
//BMP085
#define BMP085 (1)
#define BMP085_TEMP (1)
#define BMP085_PRESS (1)
#define ALT_RAW (1)
#define ALT_FILTER (1)
#define APOGEE (1)
//Giroscopio
#define GIRO (0)
#define GIRO_X (0)
#define GIRO_Y (0)
#define GIRO_Z (0)
//Magnetometro
#define MAGNETO (0)
#define MAGNETO_X (0)
#define MAGNETO_Y (0)
#define MAGNETO_Z (0)
//Acelerometro
#define ACELERO (0)
#define ACELERO_X (0)
#define ACELERO_Y (0)
#define ACELERO_Z (0)

// ********** PARAQUEDAS ********** //
#if PARA
#define NUM_PARAQUEDAS 4
Paraquedas paraquedas[NUM_PARAQUEDAS] = {  //{pino, altitude paraquedas, atraso, intervalo que ficará acionado}
  Paraquedas(IGN_1, 0, 0, 5000),
  Paraquedas(IGN_2, 0, 3000, 5000),
  Paraquedas(IGN_3, -2, 0, 5000),
  Paraquedas(IGN_4, -2, 3000, 5000)
};
#endif

// ********** SD Card ********** //
#define CHIP_SELECT 53
int fileNum = 0;
String sdName = "Math";
String fileName;

// ********** Altitude, Filtros e Apogeu ********** //
#if BMP085
Barometro barometro;
//Adafruit_BMP085 bmp;
float altInicial = 0;
#define NUM_LEITURAS_INICIAL 25

// *** Filtros **** //
const int NUM_FILTROS = 3;
Filtro filtros[NUM_FILTROS];

// *** Apogeu **** //
Apogeu apogeu;
#endif

// ********** Gyro + Mag + Accel ********** //
#if GIRO
L3G gyro;
#endif

#if MAGNETO
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(1337);
#endif

#if ACELERO
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(1338);
#endif

String dadosString = "";

void setup() {
  Serial.begin(115200);
  Wire.begin();
  dadosString = "Time (s)\t";

// ********** Iniciando os Sensores ********** //
//BME085
#if BMP085
  if (barometro.begin()) {
    Serial.println("BMP085 OK");
  } else {
    Serial.println("BMP085 not OK");
  }
  dadosString += "Temperature (*C)\tPressure (Pa)\tAltitude (m)\t";
  for (int i = 0; i < NUM_FILTROS; i++) {
    dadosString += "Altitude + Filter" + String(i + 1) + " (m)\t";  //Adiciona o cabeçalho para cada filtro
  }
  dadosString += "Apogee (0 ou 1)\t";
#endif

  // ********** Setando os Paraquedas ********** //
#if PARA
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    paraquedas[i].iniciar();
    dadosString += "Parachute" + String(i + 1) + " (bool)\t";
  }
#endif

//Giroscópio
#if GIRO
  if (!gyro.init()) {
    Serial.println("No Gyro detected");
  }
  gyro.enableDefault();
  dadosString += "GyroX (dps)\tGyroY (dps)\tGyroZ (dps)\t";
#endif

//Magnetômetro
#if MAGNETO
  if (!mag.begin()) {
    Serial.println("No HMC5883 detected");
  }
  dadosString += "MagX (uT)\t MagY (uT)\t MagZ(uT)\t";
#endif

//Acelerômetro
#if ACELERO
  if (!accel.begin()) {
    Serial.println("No ADXL345 detected");
  }
  dadosString += "AccelX (m/s^2)\tAccelY (m/s^2)\tAccelZ (m/s^2)\n";
#endif

//SDCard
#if SDCARD
  if (!SD.begin(chipSelect)) {  // see if the card is present and can be initialized:
    Serial.println("Card failed, or not present");
  }
#endif

  // ********** Filtros ********** //
#if BMP085
  //Leituras iniciais
  float somaAltInicial = 0;
  for (int i = 0; i < NUM_LEITURAS_INICIAL; i++) {
    somaAltInicial += barometro.getAltitude();
  }

  altInicial = somaAltInicial / NUM_LEITURAS_INICIAL;  //Médias das leituras iniciais
#endif

  // ********** Cabeçalho ********** //
  dadosString += "\n";
  Serial.println(dadosString);
// ********** Criando .txt no SD Card ********** //
#if SDCARD
  do {
    String fileNumString = String(fileNum);
    int numZeros = 8 - sdName.length() - fileNumString.length();
    String zeros = "";
    for (int i = 0; i < numZeros; i++) {
      zeros += "0";
    }
    fileName = sdName + zeros + fileNumString + ".txt";
    fileNum++;
  } while (SD.exists(fileName));

  Serial.println(fileName);

  File dadosFile = SD.open(fileName, FILE_WRITE);
  if (dadosFile) {
    dadosFile.println(dadosString);
    dadosFile.close();
  } else {
    Serial.println("error opening");
  }
#endif
}

void loop() {
  String dadosString = "";
  unsigned long currentMillis = millis();

// ********** BME085 - Altura e Filtros ********** //
#if BMP085
  bool getAll = barometro.getAll();
  float temperature = barometro.getTemperatura();
  int pressure = barometro.getPressao();
  float rawAltitude = barometro.getAltitude() - altInicial;

  // *** Filtros **** //
  float filteredAltitude = rawAltitude;
  for (int i = 0; i < NUM_FILTROS; i++) {
    filteredAltitude = filtros[i].atualizarFiltro(filteredAltitude);
  }

  // ********** Apogeu ********** //
  if (!apogeu.getApogeu()) {  // Chamar detectar se o apogeu ainda não foi atingido
    apogeu.detectar(filteredAltitude);
  }
#endif

// ********** Ativando os Paraquedas 1/2/3/4 ********** //
#if PARA
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    paraquedas[i].gerenciar(apogeu.getApogeu(), filteredAltitude, currentMillis);
  }
#endif

// ********** Gyro, Mag e Accel ********** //
#if GIRO
  gyro.read();
#endif

#if MAGNETO
  sensors_event_t magEvent;
  mag.getEvent(&magEvent);
#endif

#if ACELERO
  sensors_event_t accelEvent;
  accel.getEvent(&accelEvent);
#endif

  // ********** Leitura dos dados ********** //
  dadosString += String(currentMillis / 1000.0) + "\t";  //Tempo atual
#if BMP085
#if BMP085_TEMP
  dadosString += String(temperature) + "\t";  //Temperatura *C
#endif
#if BMP085_PRESS
  dadosString += String(pressure) + "\t";  //Pressão Pa
#endif
#if ALT_RAW
  dadosString += String(rawAltitude) + "\t";  //Altura com 0 filtro
#endif
#if ALT_FILTER
  for (int i = 0; i < NUM_FILTROS; i++) {
    dadosString += String(filtros[i].getFiltro()) + "\t";  //Resultado do filtro i
  }
#endif
#if APOGEE
  dadosString += String(apogeu.getApogeu()) + "\t";  //Bool indicando se o apogeu foi atingido (0 = não; 1 = sim)
#endif
#endif
#if PARA
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    dadosString += String(paraquedas[i].getParaquedas()) + "\t";
  }
#endif
#if GIRO
#if GIRO_X
  dadosString += String(gyro.g.x) + "\t";  //Rotação angular do Giroscópio no eixo x
#endif
#if GIRO_Y
  dadosString += String(gyro.g.y) + "\t";  //Rotação angular do Giroscópio no eixo y
#endif
#if GIRO_Z
  dadosString += String(gyro.g.z) + "\t";  //Rotação angular do Giroscópio no eixo z
#endif
#endif
#if MAGNETO
#if MAGNETO_X
  dadosString += String(magEvent.magnetic.x) + "\t";  //Intensidade do campo magnético no eixo x
#endif
#if MAGNETO_Y
  dadosString += String(magEvent.magnetic.y) + "\t";  //Intensidade do campo magnético no eixo y
#endif
#if MAGNETO_Z
  dadosString += String(magEvent.magnetic.z) + "\t";  //Intensidade do campo magnético no eixo z
#endif
#endif
#if ACELERO
#if ACELERO_X
  dadosString += String(accelEvent.acceleration.x) + "\t";  //Aceleração ao longo dos eixos x
#endif
#if ACELERO_Y
  dadosString += String(accelEvent.acceleration.y) + "\t";  //Aceleração ao longo dos eixos x
#endif
#if ACELERO_Z
  dadosString += String(accelEvent.acceleration.z);  //Aceleração ao longo dos eixos x
#endif
#endif

// ********** Salvamento no SD Card dos dados ********** //
#if SDCARD
  File dadosFile = SD.open(fileName, FILE_WRITE);

  if (dadosFile) {
    dadosFile.println(dadosString);
    dadosFile.close();
  } else {
    Serial.println("error opening");
  }
#endif
  Serial.println(dadosString);
}