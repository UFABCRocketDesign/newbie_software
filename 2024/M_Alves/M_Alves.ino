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
class Paraquedas {
public:
  static const int ALT_PARAQUEDAS = -3;
  int pino;
  bool ativado;
  bool apogeuApenas;
  unsigned long intervaloApogeu;
  unsigned long intervaloAltitude;
  unsigned long tempoAtivacao;
  unsigned long futureMillis;
  bool paraquedasAtivado;

  // Construtor
  Paraquedas(int pino, bool apogeuApenas, unsigned long intervaloApogeu, unsigned long intervaloAltitude, unsigned long tempoAtivacao)
    : pino(pino), ativado(false), apogeuApenas(apogeuApenas), intervaloApogeu(intervaloApogeu), intervaloAltitude(intervaloAltitude), tempoAtivacao(tempoAtivacao), futureMillis(0), paraquedasAtivado(false) {
  }

  // Método para inicializar o pino do paraquedas no setup
  void iniciar() {
    pinMode(pino, OUTPUT);
    digitalWrite(pino, LOW);
  }

  // Método para gerenciar a ativação dos paraquedas
  void gerenciar(bool apogeuAtingido, float mediaAltitudeFiltrada, unsigned long currentMillis) {
    if (apogeuAtingido && !ativado && !paraquedasAtivado && (apogeuApenas || mediaAltitudeFiltrada < ALT_PARAQUEDAS)) {
      if (currentMillis >= futureMillis + (apogeuApenas ? intervaloApogeu : intervaloAltitude)) {
        digitalWrite(pino, HIGH);
        ativado = true;
        futureMillis = currentMillis + tempoAtivacao;
      }
    } else if (ativado && currentMillis >= futureMillis) {
      digitalWrite(pino, LOW);
      ativado = false;
      paraquedasAtivado = true;
    }
  }

  // Método para verificar o estado do paraquedas e printar
  bool isAtivado() {
    return ativado;
  }
};

// Variáveis globais
#define NUM_PARAQUEDAS 4
Paraquedas paraquedas[NUM_PARAQUEDAS] = {  //{pino, paraquedas de apogeu (true) ou apogeu+altura (false), intervalo para acionar após o apogeu, intervalo para acionar após o apogeu+altura, intervalo que ficará acionado}
  Paraquedas(IGN_1, true, 0, 0, 5000),
  Paraquedas(IGN_2, true, 2000, 0, 5000),
  Paraquedas(IGN_3, false, 0, 0, 5000),
  Paraquedas(IGN_4, false, 0, 2000, 5000)
};
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
class Filtro {
private:
  static const int NUM_LEITURAS = 10;
  float leituras[NUM_LEITURAS] = { 0 };
  float somaLeituras = 0;
  int indiceLeitura = 0;

public:
  float atualizarFiltro(float novaLeitura) {
    somaLeituras -= leituras[indiceLeitura];
    leituras[indiceLeitura] = novaLeitura;
    somaLeituras += novaLeitura;
    indiceLeitura = (indiceLeitura + 1) % NUM_LEITURAS;
    return somaLeituras / NUM_LEITURAS;
  }

  float getFiltro() {
    return somaLeituras / NUM_LEITURAS;
  }
};

const int NUM_FILTROS = 3;
Filtro filtros[NUM_FILTROS];

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

  // *** Filtros **** //
  float filteredAltitude = rawAltitude;
  for (int i = 0; i < NUM_FILTROS; i++) {
    filteredAltitude = filtros[i].atualizarFiltro(filteredAltitude);
  }

  // ********** Apogeu ********** //
  if (!apogeuAtingido) {  // Chamar detectarApogeu se o apogeu ainda não foi atingido
    apogeuAtingido = detectarApogeu(filteredAltitude);
  }
#endif

// ********** Ativando os Paraquedas 1/2/3/4 ********** //
#if PARA
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    paraquedas[i].gerenciar(apogeuAtingido, filteredAltitude, currentMillis);
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
  dadosString += String(apogeuAtingido) + "\t";  //Bool indicando se o apogeu foi atingido (0 = não; 1 = sim)
#endif
#endif
#if PARA
  for (int i = 0; i < NUM_PARAQUEDAS; i++) {
    dadosString += String(paraquedas[i].isAtivado()) + "\t";
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