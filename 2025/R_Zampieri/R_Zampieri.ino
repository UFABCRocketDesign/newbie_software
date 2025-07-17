#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>  //Sensor BMP
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <Wire.h>
#include <L3G.h>

#define tamanho 10  //Usado pro filtro de dados do BMP

//Pinos de Ignitores dos Paraquedas
#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/

#include <SPI.h>  //Usado no DataLogger
#include <SD.h>   //Usado no DataLogger

Adafruit_BMP085 bmp;  //Sensor BMP
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(54321);
L3G gyro;

//Declarando variáveis e arrays pro BMP
float tara = 0;
float vetor[tamanho];
float vetor2[tamanho];
int guia = 0;
int guia2 = 0;
int detectorqueda = 0;
float alturapassada = 0;
float altura_filtrada = 0;
float altura_filtrada2 = 0;

//Declaração pro detector de queda
bool queda = false;

//Declaração pro relógio (millis)
float tempo = 0;

//Declarações pro SD DataLogger
const int chipSelect = 53;
int lognumber = 0;
String nomearquivo;
String nomelog = "ZAMP";
String zerospacetext;
int zerospacelength;

/*------DECLARAÇÕES PRO PARAQUEDAS------*/
//Estado de cada paraquedas
int paraquedas1armado = 0;
int paraquedas2armado = 0;
int paraquedas3armado = 0;
int paraquedas4armado = 0;
//Tempo de ativação de cada paraquedas
unsigned long previousMillisPRQ1 = 0; 
unsigned long previousMillisPRQ2 = 0;
unsigned long previousMillisPRQ3 = 0;
unsigned long previousMillisPRQ4 = 0;
//Intervalo entre os paraquedas
const long intervalIGN = 5000;
const long intervalPRQ2 = 2000;
const long intervalPRQ4 = 2000;

void setup() {
  /*------SETUP DATALOGGER------*/
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial)
    ;

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true)
      ;
  }

  Serial.println("initialization done.");

  do {
    zerospacelength = 8 - nomelog.length() - String(lognumber).length();
    for (int i = 1; i <= zerospacelength; ++i) {
      zerospacetext += "0";
    }
    nomearquivo = nomelog + zerospacetext + String(lognumber) + ".txt";
    lognumber += 1;
    zerospacetext = "";
  } while (SD.exists(nomearquivo));

  Serial.println(nomearquivo);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(nomearquivo, FILE_WRITE);
  //FIM DO SETUP DATALOGGER

  /*------SETUP BMP------*/
  //Inicialização do sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  //

  //Reset de altura
  for (int i = 1; i <= 10; ++i) {
    tara += bmp.readAltitude();
  }
  tara /= 10;
  //

  Serial.print("tempo\t");
  Serial.print("alturapassada\t");
  Serial.print("Altura Sem Filtro\t");
  Serial.print("Altura Filtrada\t");
  Serial.print("Altura Filtrada 2\t");
  Serial.print("Detector de Queda\t");
  Serial.print("Temperatura\t");
  Serial.print("Pressão\t");
  Serial.print("paraquedasarmado\t");
  Serial.print("x (m/s^2)\t");
  Serial.print("y (m/s^2)\t");
  Serial.print("z (m/s^2)\t");
  Serial.print("Gyro x\t");
  Serial.print("Gyro y\t");
  Serial.print("Gyro z\t");
  Serial.print("Magnet x\t");
  Serial.print("Magnet y\t");
  Serial.print("Magnet z\t");
  Serial.println();
  //FIM DO SETUP BMP

  /*------SETUP ARMAZENAMENTO NO ARQUIVO------*/
  if (dataFile) {
    dataFile.print("tempo\t");
    dataFile.print("alturapassada\t");
    dataFile.print("Altura Sem Filtro\t");
    dataFile.print("Altura Filtrada\t");
    dataFile.print("Altura Filtrada 2\t");
    dataFile.print("Detector de Queda\t");
    dataFile.print("Temperatura\t");
    dataFile.print("Pressão\t");
    dataFile.print("paraquedas1armado\t");
    dataFile.print("paraquedas2armado\t");
    dataFile.print("paraquedas3armado\t");
    dataFile.print("paraquedas4armado\t");
    dataFile.print("x (m/s^2)\t");
    dataFile.print("y (m/s^2)\t");
    dataFile.print("z (m/s^2)\t");
    dataFile.print("Gyro x\t");
    dataFile.print("Gyro y\t");
    dataFile.print("Gyro z\t");
    dataFile.print("Magnet x\t");
    dataFile.print("Magnet y\t");
    dataFile.print("Magnet z\t");
    dataFile.println();
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("Erro ao abrir" + nomearquivo);
  }

  /*------PINOS DO PARAQUEDAS------*/
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);

  /*------INICIALIZAÇÃO ACELERÔMETRO------*/
  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_16_G);

  Wire.begin();

  /*------INICIALIZAÇÃO GIROSCÓPIO------*/
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }

  gyro.enableDefault();

  /*------INICIALIZAÇÃO MAGNETÔMETRO------*/
  if (!mag.begin()) {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
}

void loop() {

  /*------COLETA DE DADOS------*/
  unsigned long currentMillis = millis();
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  tempo = currentMillis / 1000.0;

  // Get a new sensor event (Acelerômetro)
  sensors_event_t event;
  accel.getEvent(&event);

  gyro.read();

  // Get a new sensor event (Magnetômetro)
  sensors_event_t eventmag;
  mag.getEvent(&eventmag);

  //*------SENSOR BMP------*/
  //FILTROS
  vetor[guia] = bmp.readAltitude() - tara;  //setup do filtro 1
  if (guia < tamanho - 1) {
    guia += 1;
  } else {
    guia = 0;
  }

  altura_filtrada = 0;                    //reset da altura pra usar no filtro 1
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 1
    altura_filtrada += vetor[i];
  }

  altura_filtrada /= tamanho;  //output do filtro 1

  vetor2[guia2] = altura_filtrada;  //setup do filtro 2
  if (guia2 < tamanho - 1) {
    guia2 += 1;
  } else {
    guia2 = 0;
  }

  altura_filtrada2 = 0;                   //reset da altura pra usar no filtro 2
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 2
    altura_filtrada2 += vetor2[i];
  }
  altura_filtrada2 /= tamanho;  //output do filtro 2

  /*------DETECÇÃO DE QUEDA------*/
  if (altura_filtrada2 < alturapassada) {  //Comparação da altura atual pós-filtros com a altura anterior ("alturapassada")
    detectorqueda += 1;
  } else {
    detectorqueda = 0;
  }
  if (detectorqueda >= 10) {
    queda = true;
  }
  alturapassada = altura_filtrada2;  //Armazenamento da altura atual para usar como "alturapassada" no próximo loop

  /*------ACIONAMENTO DE PARAQUEDAS------*/
  // Paraquedas 1
  if ((queda) && (paraquedas1armado == 0)) {
    digitalWrite(IGN_1, HIGH);
    previousMillisPRQ1 = currentMillis;
    paraquedas1armado = 1;
  }

  if ((currentMillis - previousMillisPRQ1 >= intervalIGN) && (paraquedas1armado == 1)) {
    digitalWrite(IGN_1, LOW);
    paraquedas1armado = 2;
  }

  // Paraquedas 2
  if ((queda) && (paraquedas2armado == 0)) {
    previousMillisPRQ2 = currentMillis;
    paraquedas2armado = -1;
  }

  if ((currentMillis - previousMillisPRQ2 >= intervalPRQ2) && (paraquedas2armado == -1)) {
    digitalWrite(IGN_2, HIGH);
    previousMillisPRQ2 = currentMillis;
    paraquedas2armado = 1;
  }

  if ((currentMillis - previousMillisPRQ2 >= intervalIGN) && (paraquedas2armado == 1)) {
    digitalWrite(IGN_2, LOW);
    paraquedas2armado = 2;
  }

  // Paraquedas 3
  if ((queda) && (altura_filtrada2 <= -3) && (paraquedas3armado == 0)) {
    digitalWrite(IGN_3, HIGH);
    previousMillisPRQ3 = currentMillis;
    paraquedas3armado = 1;
  }

  if ((currentMillis - previousMillisPRQ3 >= intervalIGN) && (paraquedas3armado == 1)) {
    digitalWrite(IGN_3, LOW);
    paraquedas3armado = 2;
  }

  // Paraquedas 4
  if ((queda) && (altura_filtrada2 <= -3) && (paraquedas4armado == 0)) {
    previousMillisPRQ4 = currentMillis;
    paraquedas4armado = -1;
  }

  if ((currentMillis - previousMillisPRQ4 >= intervalPRQ4) && (paraquedas4armado == -1)) {
    digitalWrite(IGN_4, HIGH);
    previousMillisPRQ4 = currentMillis;
    paraquedas4armado = 1;
  }

  if ((currentMillis - previousMillisPRQ4 >= intervalIGN) && (paraquedas4armado == 1)) {
    digitalWrite(IGN_4, LOW);
    paraquedas4armado = 2;
  }

  /*------ARMAZENAMENTO DE DADOS------*/
  //Abre o arquivo
  File dataFile = SD.open(nomearquivo, FILE_WRITE);
  // Armazenamento dos valores na dataString
  String dataString = "";
  dataString += String(tempo) + "\t";
  dataString += String(alturapassada) + "\t";
  dataString += String(vetor[guia]) + "\t";
  dataString += String(altura_filtrada) + "\t";
  dataString += String(altura_filtrada2) + "\t";
  dataString += String(detectorqueda) + "\t";
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(paraquedas1armado) + "\t";
  dataString += String(paraquedas2armado) + "\t";
  dataString += String(paraquedas3armado) + "\t";
  dataString += String(paraquedas4armado) + "\t";
  dataString += String(event.acceleration.x) + "\t";
  dataString += String(event.acceleration.y) + "\t";
  dataString += String(event.acceleration.z) + "\t";
  dataString += String((int)gyro.g.x) + "\t";
  dataString += String((int)gyro.g.y) + "\t";
  dataString += String((int)gyro.g.z) + "\t";
  dataString += String(eventmag.magnetic.x) + "\t";
  dataString += String(eventmag.magnetic.y) + "\t";
  dataString += String(eventmag.magnetic.z) + "\t";
  //Print da dataString
  Serial.println(dataString);

  /*------SALVA NO ARQUIVO------*/
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("Erro ao abrir" + nomearquivo);
  }
}
