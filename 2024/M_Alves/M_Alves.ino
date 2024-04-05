#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <L3G.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_ADXL345_U.h>

#define SDCARD (0)
#define BMP085 (0)
#define GIRO (0)
#define MAGNETO (0)
#define ACELERO (0)

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
const long interval = 10000;
unsigned long futureMillis2 = 0;
const long interval2 = 5000;
unsigned long futureMillis3 = 0;
const long interval3 = 10000;
unsigned long futureMillis4 = 0;
const long interval4 = 5000;

// ********** SD Card ********** //
const int chipSelect = 53;
int fileNum = 0;
String sdName = "Math";
String fileName;

// ********** Altitude, Filtros e Apogeu ********** //
Adafruit_BMP085 bmp;
float AltInicial = 0;
int numLeiturasInicial = 25;
float somaAltInicial = 0;

// *** Filtro 1 **** //
const int numLeituras = 15;   // número de leituras para a média
float leituras[numLeituras];  // as leituras do sensor de altitude
int indiceLeitura = 0;        // o índice da leitura atual
float somaLeituras = 0;       // a soma das leituras
float mediaAltitude = 0;      // a média das leituras

// *** Filtro 2 **** //
float leiturasFiltradas[numLeituras];  // as leituras filtradas
float somaLeiturasFiltradas = 0;       // a soma das leituras filtradas
float mediaAltitudeFiltrada = 0;       // a média das leituras filtradas

// *** Apogeu **** //
float altitudeAnterior = -1;
int contador = 0;
int estado = 0;  // estado 0 -> subindo; estado 1 -> descendo
bool apogeu = false;

// ********** Gyro + Mag + Accel ********** //
L3G gyro;
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(1337);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(1338);

void setup() {
  Serial.begin(115200);

  // ********** Iniciando os Sensores ********** //
  //BME085
  #if BMP085
  if (!bmp.begin()) {
    Serial.println("No BMP085 detected");
  }
  #endif
  //Giroscópio
  #if GIRO
  Wire.begin();
  if (!gyro.init()) {
    Serial.println("No Gyro detected");
  }
  gyro.enableDefault();
  #endif
  //Magnetômetro
  #if MAGNETO
  if (!mag.begin()) {
    Serial.println("No HMC5883 detected");
  }
  #endif
  //Acelerômetro
  #if ACELERO
  if (!accel.begin()) {
    Serial.println("No ADXL345 detected");
  }
  #endif
  //SDCard
  #if SDCARD
  if (!SD.begin(chipSelect)) {  // see if the card is present and can be initialized:
    Serial.println("Card failed, or not present");
  }
  #endif

  // ********** Setando os Paraquedas ********** //
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
  for (int i = 0; i < numLeiturasInicial; i++) {
    somaAltInicial += bmp.readAltitude();
  }

  AltInicial = somaAltInicial / numLeiturasInicial;  //Médias das leituras iniciais

  //Armazena leituras para realizar a média
  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - AltInicial;
    somaLeituras += leituras[i];
    leiturasFiltradas[i] = leituras[i];
    somaLeiturasFiltradas += leiturasFiltradas[i];
  }

  // ********** Cabeçalho ********** //
  String dadosString = "";
  dadosString += "Time (s)\tTemperature (*C)\tPressure (Pa)\tAltitude (m)\tAltitude + Filter1 (m)\tAltitude + Filter2 (m)\tApogee (0 ou 1)\tParachute1 (bool)\tParachute2 (bool)\tParachute3 (bool)\tParachute4 (bool)\tGyroX (dps)\tGyroY (dps)\tGyroZ (dps)\tMagX (uT)\t MagY (uT)\t MagZ(uT)\tAccelX (m/s^2)\tAccelY (m/s^2)\tAccelZ (m/s^2)\n";
  Serial.println("Time (s)\tTemperature (*C)\tPressure (Pa)\tAltitude (m)\tAltitude + Filter1 (m)\tAltitude + Filter2 (m)\tApogee (0 ou 1)\tParachute1 (bool)\tParachute2 (bool)\tParachute3 (bool)\tParachute4 (bool)\tGyroX (dps)\tGyroY (dps)\tGyroZ (dps)\tMagX (uT)\t MagY (uT)\t MagZ(uT)\tAccelX (m/s^2)\tAccelY (m/s^2)\tAccelZ (m/s^2)");

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
  float rawAltitude = bmp.readAltitude() - AltInicial;

  // *** Filtro 1 **** //
  somaLeituras = somaLeituras - leituras[indiceLeitura];
  leituras[indiceLeitura] = rawAltitude;
  somaLeituras = somaLeituras + leituras[indiceLeitura];
  mediaAltitude = somaLeituras / numLeituras;


  // *** Filtro 2 **** //
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
  gyro.read();

  sensors_event_t magEvent;
  mag.getEvent(&magEvent);

  sensors_event_t accelEvent;
  accel.getEvent(&accelEvent);

  // ********** Leitura dos dados ********** //
  dadosString += String(currentMillis / 1000.0) + "\t";     //Tempo atual
  dadosString += String(bmp.readTemperature()) + "\t";      //Temperatura *C
  dadosString += String(bmp.readPressure()) + "\t";         //Pressão Pa
  dadosString += String(rawAltitude) + "\t";                //Altura com 0 filtro
  dadosString += String(mediaAltitude) + "\t";              //Altura com 1 filtro
  dadosString += String(mediaAltitudeFiltrada) + "\t";      //Altura com 2 filtros
  dadosString += String(estado) + "\t";                     //Bool indicando se o apogeu foi atingido (0 = não; 1 = sim)
  dadosString += String(digitalRead(IGN_1)) + "\t";         //Estado do Paraquedas 1 (0 = desativado; 1 = ativado)
  dadosString += String(digitalRead(IGN_2)) + "\t";         //Estado do Paraquedas 2 (0 = desativado; 1 = ativado)
  dadosString += String(digitalRead(IGN_3)) + "\t";         //Estado do Paraquedas 3 (0 = desativado; 1 = ativado)
  dadosString += String(digitalRead(IGN_4)) + "\t";         //Estado do Paraquedas 4 (0 = desativado; 1 = ativado)
  dadosString += String(gyro.g.x) + "\t";                   //Rotação angular do Giroscópio no eixo x
  dadosString += String(gyro.g.y) + "\t";                   //Rotação angular do Giroscópio no eixo y
  dadosString += String(gyro.g.z) + "\t";                   //Rotação angular do Giroscópio no eixo z
  dadosString += String(magEvent.magnetic.x) + "\t";        //Intensidade do campo magnético no eixo x
  dadosString += String(magEvent.magnetic.y) + "\t";        //Intensidade do campo magnético no eixo y
  dadosString += String(magEvent.magnetic.z) + "\t";        //Intensidade do campo magnético no eixo z
  dadosString += String(accelEvent.acceleration.x) + "\t";  //Aceleração ao longo dos eixos x
  dadosString += String(accelEvent.acceleration.y) + "\t";  //Aceleração ao longo dos eixos x
  dadosString += String(accelEvent.acceleration.z);         //Aceleração ao longo dos eixos x

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