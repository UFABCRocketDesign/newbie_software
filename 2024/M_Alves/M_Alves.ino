#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <L3G.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_ADXL345_U.h>

#define SDCARD (0)
//Paraquedas
#define PARA (1)
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
#define altFilter2 (1)
#define apogee (1)
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
#if PARA
#if PARA1
#define IGN_1 36 /*act1*/
#endif
#if PARA2
#define IGN_2 61 /*act2*/
#endif
#if PARA3
#define IGN_3 46 /*act3*/
#endif
#if PARA4
#define IGN_4 55 /*act4*/
#endif

#define interval1 10000
#define interval2 5000
#define altParaquedas -5

void gerenciarParaquedas(bool apogeuAtingido, float mediaAltitudeFiltrada, unsigned long currentMillis) {
  static bool ativacao1 = false;
  static bool ativacao2 = false;
  static bool ativacao3 = false;
  static bool ativacao4 = false;

  static bool paraquedas1Ativado = false;
  static bool paraquedas2Ativado = false;
  static bool paraquedas3Ativado = false;
  static bool paraquedas4Ativado = false;

  static unsigned long futureMillis1 = 0;
  static unsigned long futureMillis2 = 0;
  static unsigned long futureMillis3 = 0;
  static unsigned long futureMillis4 = 0;

  static unsigned long apogeuMillis = 0;         // Momento em que o apogeu foi atingido
  static unsigned long altParaquedasMillis = 0;  // Momento em que a altitude se tornou menor que "altParaquedas"

  if (apogeuAtingido == true && apogeuMillis == 0) {
    apogeuMillis = currentMillis;  // Atualize o momento em que o apogeu foi atingido
  }

  if (mediaAltitudeFiltrada < altParaquedas && altParaquedasMillis == 0) {
    altParaquedasMillis = currentMillis;  // Atualize o momento em que a altitude se tornou menor que "altParaquedas"
  }

// *** Paraquedas 1 **** //
#if PARA1
  if (apogeuAtingido == true && ativacao1 == false && paraquedas1Ativado == false) {
    digitalWrite(IGN_1, HIGH);
    ativacao1 = true;
    futureMillis1 = currentMillis + interval1;
  } else if (ativacao1 == true && currentMillis >= futureMillis1) {
    digitalWrite(IGN_1, LOW);
    ativacao1 = false;
    paraquedas1Ativado = true;
  }
#endif
// *** Paraquedas 2 **** //
#if PARA2
  if (apogeuAtingido == true && ativacao2 == false && paraquedas2Ativado == false && currentMillis >= apogeuMillis + interval2) {
    digitalWrite(IGN_2, HIGH);
    ativacao2 = true;
    futureMillis2 = currentMillis + interval1;
  } else if (ativacao2 == true && currentMillis >= futureMillis2) {
    digitalWrite(IGN_2, LOW);
    ativacao2 = false;
    paraquedas2Ativado = true;
  }
#endif
// *** Paraquedas 3 **** //
#if PARA3
  if (apogeuAtingido == true && ativacao3 == false && paraquedas3Ativado == false && mediaAltitudeFiltrada < altParaquedas) {
    digitalWrite(IGN_3, HIGH);
    ativacao3 = true;
    futureMillis3 = currentMillis + interval1;
  } else if (ativacao3 == true && currentMillis >= futureMillis3) {
    digitalWrite(IGN_3, LOW);
    ativacao3 = false;
    paraquedas3Ativado = true;
  }
#endif
// *** Paraquedas 4 **** //
#if PARA4
  if (apogeuAtingido == true && ativacao4 == false && paraquedas4Ativado == false && mediaAltitudeFiltrada < altParaquedas && currentMillis >= altParaquedasMillis + interval2) {
    digitalWrite(IGN_4, HIGH);
    ativacao4 = true;
    futureMillis4 = currentMillis + interval1;
  } else if (ativacao4 == true && currentMillis >= futureMillis4) {
    digitalWrite(IGN_4, LOW);
    ativacao4 = false;
    paraquedas4Ativado = true;
  }
#endif
}
#endif

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

// *** Filtros **** //
#define NUM_FILTROS 2
#define NUM_LEITURAS 15

float leituras[NUM_FILTROS][NUM_LEITURAS] = { 0 };
float somaLeituras[NUM_FILTROS] = { 0 };
int indiceLeitura[NUM_FILTROS] = { 0 };

float atualizarFiltro(int filtro, float novaLeitura) {
  somaLeituras[filtro] -= leituras[filtro][indiceLeitura[filtro]];
  leituras[filtro][indiceLeitura[filtro]] = novaLeitura;
  somaLeituras[filtro] += novaLeitura;
  indiceLeitura[filtro] = (indiceLeitura[filtro] + 1) % NUM_LEITURAS;
  return somaLeituras[filtro] / NUM_LEITURAS;
}

// *** Apogeu **** //
bool apogeuAtingido = false;  // Variável global para rastrear se o apogeu foi atingido
bool detectarApogeu(float alturaAtual) {
  static float altitudeAnterior = -1;
  static int contador = 0;
  static int estado = 0;  // estado 0 -> subindo; estado 1 -> descendo
  static bool apogeu = false;

  if (altitudeAnterior != -1 && alturaAtual < altitudeAnterior) {
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

  altitudeAnterior = alturaAtual;  // Atualize a altitude anterior para a próxima iteração
  return apogeu;
}
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
  dadosString = "Time (s)\t";

// ********** Iniciando os Sensores ********** //
//BME085
#if BMP085
  if (!bmp.begin()) {
    Serial.println("No BMP085 detected");
  }
  dadosString += "Temperature (*C)\tPressure (Pa)\tAltitude (m)\tAltitude + Filter1 (m)\tAltitude + Filter2 (m)\tApogee (0 ou 1)\t";
#endif

  // ********** Setando os Paraquedas ********** //
#if PARA
#if PARA1
  pinMode(IGN_1, OUTPUT);
  digitalWrite(IGN_1, LOW);
  dadosString += "Parachute1 (bool)\t";
#endif
#if PARA2
  pinMode(IGN_2, OUTPUT);
  digitalWrite(IGN_2, LOW);
  dadosString += "Parachute2 (bool)\t";
#endif
#if PARA3
  pinMode(IGN_3, OUTPUT);
  digitalWrite(IGN_3, LOW);
  dadosString += "Parachute3 (bool)\t";
#endif
#if PARA4
  pinMode(IGN_4, OUTPUT);
  digitalWrite(IGN_4, LOW);
  dadosString += "Parachute4 (bool)\t";
#endif
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

  // ********** Filtros ********** //
#if BMP085
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
  float mediaAltitude = atualizarFiltro(0, rawAltitude);

  // *** Filtro 2 **** //
  float mediaAltitudeFiltrada = atualizarFiltro(1, mediaAltitude);

  // ********** Apogeu ********** //
  if (!apogeuAtingido) {  // Chamar detectarApogeu se o apogeu ainda não foi atingido
    apogeuAtingido = detectarApogeu(mediaAltitudeFiltrada);
  }
#endif

  // ********** Ativando os Paraquedas 1/2/3/4 ********** //
#if PARA
  gerenciarParaquedas(apogeuAtingido, mediaAltitudeFiltrada, currentMillis);
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
#if BMP085temp
  dadosString += String(temperature) + "\t";  //Temperatura *C
#endif
#if BMP085press
  dadosString += String(pressure) + "\t";  //Pressão Pa
#endif
#if altRAW
  dadosString += String(rawAltitude) + "\t";  //Altura com 0 filtro
#endif
#if altFilter1
  dadosString += String(mediaAltitude) + "\t";  //Altura com 1 filtro
#endif
#if altFilter2
  dadosString += String(mediaAltitudeFiltrada) + "\t";  //Altura com 2 filtro
#endif
#if apogee
  dadosString += String(apogeuAtingido) + "\t";  //Bool indicando se o apogeu foi atingido (0 = não; 1 = sim)
#endif
#endif
#if PARA
#if PARA1
  dadosString += String(digitalRead(IGN_1)) + "\t";  //Estado do Paraquedas 1 (0 = desativado; 1 = ativado)
#endif
#if PARA2
  dadosString += String(digitalRead(IGN_2)) + "\t";  //Estado do Paraquedas 2 (0 = desativado; 1 = ativado)
#endif
#if PARA3
  dadosString += String(digitalRead(IGN_3)) + "\t";  //Estado do Paraquedas 3 (0 = desativado; 1 = ativado)
#endif
#if PARA4
  dadosString += String(digitalRead(IGN_4)) + "\t";  //Estado do Paraquedas 4 (0 = desativado; 1 = ativado)
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