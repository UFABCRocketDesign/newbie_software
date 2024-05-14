#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <L3G.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_ADXL345_U.h>

#define SDCARD (1)
//Paraquedas
#define state (1)
#define PARA1 (1)
#define PARA2 (1)
#define PARA3 (1)
#define PARA4 (1)
//BMP085
#define BMP085 (1)
#define BMP085temp (1)
#define BMP085press (1)
#define altRAW (1)
#define altFilter1 (1)
#define altFilte2 (1)
//Giroscopio
#define GIRO (1)
#define GIROX (1)
#define GIROY (1)
#define GIROZ (1)
//Magnetometro
#define MAGNETO (1)
#define MAGNETOX (1)
#define MAGNETOY (1)
#define MAGNETOZ (1)
//Acelerometro
#define ACELERO (1)
#define ACELEROX (1)
#define ACELEROY (1)
#define ACELEROZ (1)

// ********** PARAQUEDAS ********** //
#define IGN_1 36 /*act1*/
bool ativacao1 = false;
#define IGN_2 61 /*act2*/
bool ativacao2 = false;
#define IGN_3 46 /*act3*/
bool ativacao3 = false;
#define IGN_4 55 /*act4*/
bool ativacao4 = false;

unsigned long futureMillis = 0;
#define interval 10000
unsigned long futureMillis2 = 0;
#define interval2 5000
unsigned long futureMillis3 = 0;
#define interval3 10000
unsigned long futureMillis4 = 0;
#define interval4 5000

// ********** SD Card ********** //
#define chipSelect 53
int fileNum = 0;
String sdName = "Math";
String fileName;

// ********** Altitude, Filtros e Apogeu ********** //
#if BMP085
Adafruit_BMP085 bmp;
float AltInicial = 0;
#define numLeiturasInicial 25

// *** Filtro 1 **** //
#define numLeituras 15        // número de leituras para a média
float leituras[numLeituras];  // as leituras do sensor de altitude
int indiceLeitura = 0;        // o índice da leitura atual
float somaLeituras = 0;       // a soma das leituras

// *** Filtro 2 **** //
float leiturasFiltradas[numLeituras];  // as leituras filtradas
float somaLeiturasFiltradas = 0;       // a soma das leituras filtradas

// *** Apogeu **** //
float altitudeAnterior = -1;
int contador = 0;
int estado = 0;  // estado 0 -> subindo; estado 1 -> descendo
bool apogeu = false;
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

void setup() {
  Serial.begin(115200);
  String dadosString = "Time (s)\t";

// ********** Iniciando os Sensores ********** //
//BME085
#if BMP085
  if (!bmp.begin()) {
    Serial.println("No BMP085 detected");
  }
  dadosString += "Temperature (*C)\tPressure (Pa)\tAltitude (m)\tAltitude + Filter1 (m)\tAltitude + Filter2 (m)\tApogee (0 ou 1)\tParachute1 (bool)\tParachute2 (bool)\tParachute3 (bool)\tParachute4 (bool)\t";
#endif
//Giroscópio
#if GIRO
  Wire.begin();
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

  // ********** Setando os Paraquedas ********** //
#if BMP085
  pinMode(IGN_1, OUTPUT);
  digitalWrite(IGN_1, LOW);
  pinMode(IGN_2, OUTPUT);
  digitalWrite(IGN_2, LOW);
  pinMode(IGN_3, OUTPUT);
  digitalWrite(IGN_3, LOW);
  pinMode(IGN_4, OUTPUT);
  digitalWrite(IGN_4, LOW);

  // ********** Filtros ********** //
  //Leituras iniciais
  float somaAltInicial = 0;
  for (int i = 0; i < numLeiturasInicial; i++) {
    somaAltInicial += bmp.readAltitude();
  }

  AltInicial = somaAltInicial / numLeiturasInicial;  //Médias das leituras iniciais
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
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float rawAltitude = bmp.readAltitude() - AltInicial;

  // *** Filtro 1 **** //
  float mediaAltitude;
  somaLeituras = somaLeituras - leituras[indiceLeitura];
  leituras[indiceLeitura] = rawAltitude;
  somaLeituras = somaLeituras + leituras[indiceLeitura];
  mediaAltitude = somaLeituras / numLeituras;


  // *** Filtro 2 **** //
  float mediaAltitudeFiltrada;
  somaLeiturasFiltradas = somaLeiturasFiltradas - leiturasFiltradas[indiceLeitura];
  leiturasFiltradas[indiceLeitura] = mediaAltitude;  // usa a média do primeiro filtro
  somaLeiturasFiltradas = somaLeiturasFiltradas + leiturasFiltradas[indiceLeitura];
  mediaAltitudeFiltrada = somaLeiturasFiltradas / numLeituras;

  indiceLeitura++;
  if (indiceLeitura >= numLeituras) {  //se for o último vetor, volta para o início
    indiceLeitura = 0;
  }

  // ********** Apogeu ********** //
  if (altitudeAnterior != -1 && mediaAltitudeFiltrada < altitudeAnterior) {
    contador++;
    if (contador >= 25) {
      estado = 1;
      if (estado == 1) {
        apogeu = true;
      }
    }
  } else {
    contador = 0;
    estado = 0;
  }

  altitudeAnterior = mediaAltitudeFiltrada;  // Atualize a altitude anterior para a próxima iteração
#endif
  // ********** Ativando os Paraquedas 1/2/3/4 ********** //
  // *** Paraquedas 1 **** //
  if (apogeu == true && ativacao1 == false) {
    digitalWrite(IGN_1, HIGH);
    ativacao1 = true;
    ativacao2 = true;
    futureMillis = currentMillis + interval;
    futureMillis2 = currentMillis + interval2;
  }

  // *** Paraquedas 2 **** //
  if (ativacao2 == true && currentMillis >= futureMillis2) {
    digitalWrite(IGN_2, HIGH);
    ativacao2 = false;
    futureMillis2 = currentMillis + interval;
  }

  // *** Paraquedas 3 **** //
  if (apogeu == true && ativacao3 == false && mediaAltitudeFiltrada < -5) {
    digitalWrite(IGN_3, HIGH);
    ativacao3 = true;
    ativacao4 = true;
    futureMillis3 = currentMillis + interval3;
    futureMillis4 = currentMillis + interval4;
  }

  // *** Paraquedas 4 **** //
  if (ativacao4 == true && currentMillis >= futureMillis4) {
    digitalWrite(IGN_4, HIGH);
    ativacao4 = false;
    futureMillis4 = currentMillis + interval3;
  }

  // ********** Desativando os Paraquedas 1/2/3/4 ********** //
  if (currentMillis >= futureMillis) {
    digitalWrite(IGN_1, LOW);
  }

  if (currentMillis >= futureMillis2) {
    digitalWrite(IGN_2, LOW);
  }

  if (currentMillis >= futureMillis3) {
    digitalWrite(IGN_3, LOW);
  }

  if (currentMillis >= futureMillis4) {
    digitalWrite(IGN_4, LOW);
  }

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
#if BMP085temp
  dadosString += String(temperature) + "\t";  //Temperatura *C
#endif
#if BMP085press
  dadosString += String(pressure) + "\t";  //Pressão Pa
#endif
#if altRAW
  dadosString += String(rawAltitude) + "\t";            //Altura com 0 filtro
  #endif
  #if altFilter1
  dadosString += String(mediaAltitude) + "\t";          //Altura com 1 filtro
  #endif
  #if altFilter2
  dadosString += String(mediaAltitudeFiltrada) + "\t";  //Altura com 2 filtro
  #endif
  #if state
  dadosString += String(estado) + "\t";                 //Bool indicando se o apogeu foi atingido (0 = não; 1 = sim)
  #endif
  #if PARA1
  dadosString += String(digitalRead(IGN_1)) + "\t";     //Estado do Paraquedas 1 (0 = desativado; 1 = ativado)
  #endif
  #if PARA2
  dadosString += String(digitalRead(IGN_2)) + "\t";     //Estado do Paraquedas 2 (0 = desativado; 1 = ativado)
  #endif
  #if PARA3
  dadosString += String(digitalRead(IGN_3)) + "\t";     //Estado do Paraquedas 3 (0 = desativado; 1 = ativado)
  #endif
  #if PARA4
  dadosString += String(digitalRead(IGN_4)) + "\t";     //Estado do Paraquedas 4 (0 = desativado; 1 = ativado)
  #endif
#endif
#if GIRO
#if GIROX
  dadosString += String(gyro.g.x) + "\t";  //Rotação angular do Giroscópio no eixo x
#endif
#if GIROY
  dadosString += String(gyro.g.y) + "\t";  //Rotação angular do Giroscópio no eixo y
#endif
#if GIROZ
  dadosString += String(gyro.g.z) + "\t";  //Rotação angular do Giroscópio no eixo z
#endif
#endif
#if MAGNETO
#if MAGNETOX
  dadosString += String(magEvent.magnetic.x) + "\t";  //Intensidade do campo magnético no eixo x
#endif
#if MAGNETOY
  dadosString += String(magEvent.magnetic.y) + "\t";  //Intensidade do campo magnético no eixo y
#endif
#if MAGNETOZ
  dadosString += String(magEvent.magnetic.z) + "\t";  //Intensidade do campo magnético no eixo z
#endif
#endif
#if ACELERO
#if ACELEROX
  dadosString += String(accelEvent.acceleration.x) + "\t";  //Aceleração ao longo dos eixos x
#endif
#if ACELEROY
  dadosString += String(accelEvent.acceleration.y) + "\t";  //Aceleração ao longo dos eixos x
#endif
#if ACELEROZ
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