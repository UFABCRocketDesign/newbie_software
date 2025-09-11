#include <SPI.h>
#include <SD.h>
#include <Wire.h>


#define GPS_HABILITAR 1
#define LORA_HABILITAR 0
#define ACCEL_HABILITAR 1
#define ACCEL_X_HABILITAR (ACCEL_HABILITAR && 1)
#define ACCEL_Y_HABILITAR (ACCEL_HABILITAR && 1)
#define ACCEL_Z_HABILITAR (ACCEL_HABILITAR && 1)
#define MAG_HABILITAR 1
#define MAG_X_HABILITAR (MAG_HABILITAR && 1)
#define MAG_Y_HABILITAR (MAG_HABILITAR && 1)
#define MAG_Z_HABILITAR (MAG_HABILITAR && 1)
#define GYRO_HABILITAR 1
#define GYRO_X_HABILITAR (GYRO_HABILITAR && 1)
#define GYRO_Y_HABILITAR (GYRO_HABILITAR && 1)
#define GYRO_Z_HABILITAR (GYRO_HABILITAR && 1)
#define BMP_HABILITAR 1
#define TEMP_HABILITAR (BMP_HABILITAR && 0)
#define BARO_HABILITAR (BMP_HABILITAR && 1)
#define PRESS_HABILITAR (BMP_HABILITAR && 0)
#define PQUEDAS_HABILITAR (BARO_HABILITAR && 1)
#define CHIP_HABILITAR 0

#if GPS_HABILITAR
HardwareSerial &GPS = Serial1;
#include <TinyGPSPlus.h>
TinyGPSPlus gps;
#endif

#include <Adafruit_Sensor.h>

#if LORA_HABILITAR
HardwareSerial &LoRa(Serial3);
#define LORA_ESPERA 3000
long int timerLora = 0;
#endif

#if ACCEL_HABILITAR
#include <Adafruit_ADXL345_U.h>
#endif

#if MAG_HABILITAR
#include <Adafruit_HMC5883_U.h>
#endif

#if BMP_HABILITAR
#include <Adafruit_BMP085.h>
#endif

#if GYRO_HABILITAR
#include <L3G.h>
#endif

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif  // ARDUINO_AVR_MEGA2560

#if PQUEDAS_HABILITAR
#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/
#define INTER1 5000
#define INTER2 5000
#define INTER3 5000
#define INTER4 5000
#define COMPARACAO_APOGEU_ALTURA -3
#endif

#if CHIP_HABILITAR
#define SD_HABILITAR 53
#define MAX_TAM_SD 8
#endif

#if BARO_HABILITAR
#define NUMERO_DE_FILTRAGENS 4
#define LARGURA_DA_FILTARGEM 9
#define COMPARACAO_PARA_APOGEU 14

//VER SE N FIZ CAGADA
#define VALOR_P_MEDIO_ALTURA 10
#endif
// Arrumar os define
#if CHIP_HABILITAR
File dataFile;
String docName = "";
#endif

#if MAG_HABILITAR
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
#endif
#if ACCEL_HABILITAR
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
#endif
#if BMP_HABILITAR
Adafruit_BMP085 bmp;
#endif
#if GYRO_HABILITAR
L3G gyro;
#endif

#if BMP_HABILITAR
float mediaAltura = 0;

#endif
//mudar
#if PQUEDAS_HABILITAR
uint8_t IGN[4] = { IGN_1, IGN_2, IGN_3, IGN_4 };
long int timerParaQuedas[4] = { 0, 0, 0, 0 };
bool ocorreuApogeu = 0;
int setComComparacaoAltura[4] = { 0, 0, COMPARACAO_APOGEU_ALTURA, COMPARACAO_APOGEU_ALTURA };
bool setSemComparacaoAltura[4] = { 1, 1, 0, 0 };
int estadoTodosParaquedas[4] = { 0, 0, 0, 0 };
int intervalorEmRelacaoTimer[4] = { 0, 2000, 0, 2000 };
int intervalos[4] = { INTER1, INTER2, INTER3, INTER4 };

void setQueda(int numPaQue, long int timeGlobal, float hNow, bool apogeu) {
  if ((apogeu) && (estadoTodosParaquedas[numPaQue] == 0) && ((setComComparacaoAltura[numPaQue] >= hNow) || setSemComparacaoAltura[numPaQue])) {
    timerParaQuedas[numPaQue] = timeGlobal;
    estadoTodosParaquedas[numPaQue] = 1;
  } else if ((estadoTodosParaquedas[numPaQue] == 1) && (timeGlobal - timerParaQuedas[numPaQue] >= intervalorEmRelacaoTimer[numPaQue])) {
    digitalWrite(IGN[numPaQue], HIGH);
    estadoTodosParaquedas[numPaQue] = 2;
    timerParaQuedas[numPaQue] = timeGlobal;
  } else if ((estadoTodosParaquedas[numPaQue] == 2) && (timeGlobal - timerParaQuedas[numPaQue] >= intervalos[numPaQue])) {
    digitalWrite(IGN[numPaQue], LOW);
    estadoTodosParaquedas[numPaQue] = 3;
  }
}
#endif

#if BARO_HABILITAR


float valoresFiltrados[NUMERO_DE_FILTRAGENS + 1];
float ordemAltura[COMPARACAO_PARA_APOGEU];
bool auxiliarParaCompararAlturas;

bool detecQued(float ultAlt) {
  for (int i = COMPARACAO_PARA_APOGEU - 1; i > 0; i--) {
    ordemAltura[i] = ordemAltura[i - 1];
  }
  ordemAltura[0] = ultAlt;
  auxiliarParaCompararAlturas = true;
  for (int i = 0; i < COMPARACAO_PARA_APOGEU - 1; i++) {
    auxiliarParaCompararAlturas = auxiliarParaCompararAlturas && (ordemAltura[i] < ordemAltura[i + 1]);
  }
  return auxiliarParaCompararAlturas;
}

bool mostraAuxAltura() {
  return auxiliarParaCompararAlturas;
}

class Filtros {
private:
  float* valoresFiltros = new float[largura];
  int indiceRotacao=0;
  const int largura; 
public:
  Filtros(int larguraRecebido) : largura(larguraRecebido){
  }
  float filtro(float valorRecebido) {
    float somasFiltro;
    valoresFiltros[indiceRotacao] = valorRecebido;
    somasFiltro = 0;

    for (int i = 0; i < largura; i++) {
      somasFiltro += valoresFiltros[i];
      
    }
    indiceRotacao += 1;
    indiceRotacao %= largura;
    return somasFiltro / largura;
  }
};

//Filtros selectFilter[NUMERO_DE_FILTRAGENS];
Filtros selectFilter[4] = {Filtros(9),Filtros(9),Filtros(9),Filtros(9)};


//mudar val






#endif
void setup() {
  String saidaString = "";
#if CHIP_HABILITAR
  String nome = "leo";
  int valSd = 0;
#endif
  Serial.begin(115200);
#if LORA_HABILITAR
  LoRa.begin(9600);
#endif
#if GPS_HABILITAR
  GPS.begin(9600);
#endif
  Wire.begin();
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
#if CHIP_HABILITAR
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_HABILITAR)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }
#endif
#if BMP_HABILITAR
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif
#if ACCEL_HABILITAR
  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif
#if GYRO_HABILITAR
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }
#endif
#if MAG_HABILITAR
  if (!mag.begin()) {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif
#if CHIP_HABILITAR
  Serial.println("card initialized.");

  if (nome.length() > MAX_TAM_SD) {
    for (int i = 0; i < MAX_TAM_SD; i++) {
      nome[i] = nome[i + 1];
    }
  }


  do {
    docName = nome;

    for (int i = 0; i < (MAX_TAM_SD - nome.length()) - (String(valSd).length()); i++) {
      docName += String(0);
    }

    docName += String(valSd) + ".txt";

    valSd += 1;
  } while (SD.exists(docName));

  Serial.println("Creating " + docName + "...");
  dataFile = SD.open(docName, FILE_WRITE);
#endif

  saidaString += String("tempo\t");

#if TEMP_HABILITAR
  saidaString += String("Temperature\t");
#endif
#if PRESS_HABILITAR
  saidaString += String("Pressure\t");
#endif
#if BARO_HABILITAR
  for (int i = 0; i < NUMERO_DE_FILTRAGENS + 1; i++) {
    saidaString += String("Filtro");
    saidaString += String(i + 1) + "\t";
  }
  saidaString += String("auxiliarParaCompararAlturas\t");
#endif
#if PQUEDAS_HABILITAR
  saidaString += String("pQuedas1\tpQuedas2\tpQuedas3\tpQuedas4\t");
#endif
#if ACCEL_X_HABILITAR
  saidaString += String("acc_x\t");
#endif
#if ACCEL_Y_HABILITAR
  saidaString += String("acc_y\t");
#endif
#if ACCEL_Z_HABILITAR
  saidaString += String("acc_z\t");
#endif
#if GYRO_X_HABILITAR
  saidaString += String("gyro_x\t");
#endif
#if GYRO_Y_HABILITAR
  saidaString += String("gyro_y\t");
#endif
#if GYRO_Z_HABILITAR
  saidaString += String("gyro_z\t");
#endif
#if MAG_X_HABILITAR
  saidaString += String("mag_x\t");
#endif
#if MAG_Y_HABILITAR
  saidaString += String("mag_y\t");
#endif
#if MAG_Z_HABILITAR
  saidaString += String("mag_z\t");
#endif
#if GPS_HABILITAR
  saidaString += String("latitude\tlongitude\t");
#endif
#if CHIP_HABILITAR
  dataFile.println(saidaString);
  dataFile.close();
#endif
  Serial.println(saidaString);
#if LORA_HABILITAR
  LoRa.println(saidaString);
#endif
#if BARO_HABILITAR
  for (int i = 0; i < VALOR_P_MEDIO_ALTURA; i++) {
    mediaAltura += bmp.readAltitude();
  }
  mediaAltura /= VALOR_P_MEDIO_ALTURA;
#endif
#if PQUEDAS_HABILITAR
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
#endif
#if ACCEL_HABILITAR
  accel.setRange(ADXL345_RANGE_16_G);
#endif
#if GYRO_HABILITAR
  gyro.enableDefault();
#endif
}


void loop() {

  long int timeGlobal = millis();

  String dataString = "";
#if ACCEL_HABILITAR
  sensors_event_t eventac;
  accel.getEvent(&eventac);
#endif
#if GYRO_HABILITAR
  gyro.read();
#endif
#if MAG_HABILITAR
  sensors_event_t eventmag;
  mag.getEvent(&eventmag);
#endif
#if BARO_HABILITAR
  valoresFiltrados[0] = bmp.readAltitude() - mediaAltura;

  for (int i = 0; i < NUMERO_DE_FILTRAGENS; i++) {
    valoresFiltrados[i + 1] = selectFilter[i].filtro(valoresFiltrados[i]);

  }

  




#endif
#if PQUEDAS_HABILITAR
  if (!ocorreuApogeu) {
    if (detecQued(valoresFiltrados[NUMERO_DE_FILTRAGENS])) {
      ocorreuApogeu = 1;
    }
  }
  for (int i = 0; i < 4; i++) {
    setQueda(i, timeGlobal, valoresFiltrados[NUMERO_DE_FILTRAGENS], ocorreuApogeu);
  }
#endif

  dataString += String(timeGlobal / 1000.0);
  dataString += "\t";

#if TEMP_HABILITAR
  dataString += String(bmp.readTemperature());
  dataString += "\t";
#endif
#if PRESS_HABILITAR
  dataString += String(bmp.readPressure());
  dataString += "\t";
#endif
#if BARO_HABILITAR
  for (int i = 0; i < NUMERO_DE_FILTRAGENS + 1; i++) {
    dataString += String(valoresFiltrados[i]);
    dataString += "\t";
  }

  dataString += String(mostraAuxAltura());
  dataString += "\t";
#endif
#if PQUEDAS_HABILITAR
  dataString += String(estadoTodosParaquedas[0]);
  dataString += "\t";

  dataString += String(estadoTodosParaquedas[1]);
  dataString += "\t";

  dataString += String(estadoTodosParaquedas[2]);
  dataString += "\t";

  dataString += String(estadoTodosParaquedas[3]);
  dataString += "\t";
#endif
#if ACCEL_X_HABILITAR
  dataString += String(eventac.acceleration.x);
  dataString += "\t";
#endif
#if ACCEL_Y_HABILITAR
  dataString += String(eventac.acceleration.y);
  dataString += "\t";
#endif
#if ACCEL_Z_HABILITAR
  dataString += String(eventac.acceleration.z);
  dataString += "\t";
#endif
#if GYRO_X_HABILITAR
  dataString += String((int)gyro.g.x);
  dataString += "\t";
#endif
#if GYRO_Y_HABILITAR
  dataString += String((int)gyro.g.y);
  dataString += "\t";
#endif
#if GYRO_Z_HABILITAR
  dataString += String((int)gyro.g.z);
  dataString += "\t";
#endif
#if MAG_X_HABILITAR
  dataString += String(eventmag.magnetic.x);
  dataString += "\t";
#endif
#if MAG_Y_HABILITAR
  dataString += String(eventmag.magnetic.y);
  dataString += "\t";
#endif
#if MAG_Z_HABILITAR
  dataString += String(eventmag.magnetic.z);
  dataString += "\t";
#endif
#if GPS_HABILITAR
  while (GPS.available()) {
    gps.encode(GPS.read());
  }
  dataString += String(gps.location.lat(), 6);
  dataString += "\t";

  dataString += String(gps.location.lng(), 6);
  dataString += "\t";
#endif
  Serial.println(dataString);
#if LORA_HABILITAR
  if (timeGlobal - timerLora >= LORA_ESPERA) {
    LoRa.println(dataString);
    timerLora = timeGlobal;
  }
#endif

#if CHIP_HABILITAR


  File dataFile = SD.open(docName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
#endif
}
