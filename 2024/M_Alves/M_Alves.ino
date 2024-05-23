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
//BMP085
#define BMP085 (1)
#define BMP085_TEMP (1)
#define BMP085_PRESS (1)
#define ALT_RAW (1)
#define ALT_FILTER1 (1)
#define ALT_FILTER2 (1)
#define APOGEE (1)
//Giroscopio
#define GIRO (1)
#define GIRO_X (1)
#define GIRO_Y (1)
#define GIRO_Z (1)
//Magnetometro
#define MAGNETO (1)
#define MAGNETO_X (1)
#define MAGNETO_Y (1)
#define MAGNETO_Z (1)
//Acelerometro
#define ACELERO (1)
#define ACELERO_X (1)
#define ACELERO_Y (1)
#define ACELERO_Z (1)

// ********** PARAQUEDAS ********** //
#if PARA
#define NUM_PARAQUEDAS 4
#define ALT_PARAQUEDAS -3

int ign[NUM_PARAQUEDAS] = { 36, 61, 46, 55 };  // Número dos pinos para cada paraquedas
bool ativacao[NUM_PARAQUEDAS] = { false };
bool paraquedasAtivado[NUM_PARAQUEDAS] = { false };
unsigned long futureMillis[NUM_PARAQUEDAS] = { 0 };

bool apogeuApenas[NUM_PARAQUEDAS] = { true, true, false, false };  // Paraquedas que ativam com apenas o apogeu = true, paraquedas que precisam de uma altura específica = false

unsigned long intervalosApogeu[NUM_PARAQUEDAS] = { 0, 2000, 0, 0 };    // Intervalo de ativação entre os paraquedas de "apenas apogeu"
unsigned long intervalosAltitude[NUM_PARAQUEDAS] = { 0, 0, 0, 2000 };  // Intervalo de ativação entre os paraquedas de altura específica

unsigned long tempoAtivacao[NUM_PARAQUEDAS] = { 5000, 5000, 5000, 5000 };  // Tempo de ativação de cada paraquedas

void gerenciarParaquedasIndividual(int i, bool apogeuAtingido, float mediaAltitudeFiltrada, unsigned long currentMillis) {
  static bool altParaquedasBool = false;

  if (apogeuAtingido == true && mediaAltitudeFiltrada < ALT_PARAQUEDAS) {
    altParaquedasBool = true;
  }

  if (apogeuAtingido == true && ativacao[i] == false && paraquedasAtivado[i] == false && (apogeuApenas[i] || altParaquedasBool)) {
    if (i == 0 || (i > 0 && currentMillis >= futureMillis[i - 1] + (apogeuApenas[i] ? intervalosApogeu[i] : intervalosAltitude[i]))) {
      digitalWrite(ign[i], HIGH);
      ativacao[i] = true;
      futureMillis[i] = currentMillis + tempoAtivacao[i];
    }
  } else if (ativacao[i] == true && currentMillis >= futureMillis[i]) {
    digitalWrite(ign[i], LOW);
    ativacao[i] = false;
    paraquedasAtivado[i] = true;
  }
}
#endif

// ********** SD Card ********** //
#define CHIP_SELECT 53
int fileNum = 0;
String sdName = "Math";
String fileName;

// ********** Altitude, Filtros e Apogeu ********** //
#if BMP085
Adafruit_BMP085 bmp;
float altInicial = 0;
#define NUM_LEITURAS_INICIAL 25

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
  static float alturaAnterior = -1;
  static int contador = 0;
  static int estado = 0;  // estado 0 -> subindo; estado 1 -> descendo
  static bool apogeu = false;

  if (alturaAnterior != -1 && alturaAtual < alturaAnterior) {
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

  alturaAnterior = alturaAtual;  // Atualize a altitude anterior para a próxima iteração
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
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    pinMode(ign[i], OUTPUT);
    digitalWrite(ign[i], LOW);
    dadosString += "Parachute" + String(i + 1) + " (bool)\t";
  }
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
  for (int i = 0; i < NUM_LEITURAS_INICIAL; i++) {
    somaAltInicial += bmp.readAltitude();
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
  float temperature = bmp.readTemperature();
  int pressure = bmp.readPressure();
  float rawAltitude = bmp.readAltitude() - altInicial;

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
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    gerenciarParaquedasIndividual(i, apogeuAtingido, mediaAltitudeFiltrada, currentMillis);
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
#if ALT_FILTER1
  dadosString += String(mediaAltitude) + "\t";  //Altura com 1 filtro
#endif
#if ALT_FILTER2
  dadosString += String(mediaAltitudeFiltrada) + "\t";  //Altura com 2 filtro
#endif
#if APOGEE
  dadosString += String(apogeuAtingido) + "\t";  //Bool indicando se o apogeu foi atingido (0 = não; 1 = sim)
#endif
#endif
#if PARA
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    dadosString += String(digitalRead(ign[i])) + "\t";  //Estado do Paraquedas i (0 = desativado; 1 = ativado)
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