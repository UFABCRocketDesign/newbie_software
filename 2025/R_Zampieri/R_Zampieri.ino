#define USANDO_BMP 1
#define USANDO_BMP_ALTURA (USANDO_BMP && 1)
#define USANDO_BMP_PRESSAO (USANDO_BMP && 1)
#define USANDO_BMP_TEMPERATURA (USANDO_BMP && 1)

#define USANDO_ACELEROMETRO 1
#define USANDO_ACELEROMETRO_X (USANDO_ACELEROMETRO && 1)
#define USANDO_ACELEROMETRO_Y (USANDO_ACELEROMETRO && 1)
#define USANDO_ACELEROMETRO_Z (USANDO_ACELEROMETRO && 1)

#define USANDO_MAGNETOMETRO 1
#define USANDO_MAGNETOMETRO_X (USANDO_MAGNETOMETRO && 1)
#define USANDO_MAGNETOMETRO_Y (USANDO_MAGNETOMETRO && 1)
#define USANDO_MAGNETOMETRO_Z (USANDO_MAGNETOMETRO && 1)

#define USANDO_GIROSCOPIO 1
#define USANDO_GIROSCOPIO_X (USANDO_GIROSCOPIO && 1)
#define USANDO_GIROSCOPIO_Y (USANDO_GIROSCOPIO && 1)
#define USANDO_GIROSCOPIO_Z (USANDO_GIROSCOPIO && 1)

#define USANDO_PARAQUEDAS (USANDO_BMP_ALTURA && 1)
#define USANDO_PARAQUEDAS_1 (USANDO_PARAQUEDAS && 1)
#define USANDO_PARAQUEDAS_2 (USANDO_PARAQUEDAS && 1)
#define USANDO_PARAQUEDAS_3 (USANDO_PARAQUEDAS && 1)
#define USANDO_PARAQUEDAS_4 (USANDO_PARAQUEDAS && 1)

#define USANDO_RELOGIO (USANDO_PARAQUEDAS || 1)

#define USANDO_SD 1

/*---------------BMP---------------*/
#if USANDO_BMP
#include <Adafruit_BMP085.h>  //Sensor BMP
#define tamanho 10            //Usado pro filtro de dados do BMP
Adafruit_BMP085 bmp;          //Sensor BMP
//Declarando variáveis e arrays pro BMP
#if USANDO_BMP_ALTURA
float tara = 0;
float vetor[tamanho];
float vetor2[tamanho];
int guia = 0;
int guia2 = 0;
int detectorqueda = 0;
float alturapassada = 0;
float altura_filtrada = 0;
float altura_filtrada2 = 0;
bool queda = false;  //Declaração pro detector de queda
#endif               //ALTURA
#if USANDO_BMP_PRESSAO
int32_t pressao;
#endif  //PRESSAO
#if USANDO_BMP_TEMPERATURA
float temperatura;
#endif  //TEMPERATURA
#endif  //BMP

/*---------------ACELERÔMETRO---------------*/
#if (USANDO_ACELEROMETRO && (USANDO_ACELEROMETRO_X || USANDO_ACELEROMETRO_Y || USANDO_ACELEROMETRO_Z))
#include <Adafruit_ADXL345_U.h>  //Acelerômetro
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
#endif  //ACELEROMETRO

/*---------------MAGNETÔMETRO---------------*/
#if (USANDO_MAGNETOMETRO && (USANDO_MAGNETOMETRO_X || USANDO_MAGNETOMETRO_Y || USANDO_MAGNETOMETRO_Z))
#include <Adafruit_HMC5883_U.h>  //Magnetômetro
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(54321);
#endif  //MAGNETOMETRO

/*---------------GIROSCÓPIO---------------*/
#if (USANDO_GIROSCOPIO && (USANDO_GIROSCOPIO_X || USANDO_GIROSCOPIO_Y || USANDO_GIROSCOPIO_Z))
#include <L3G.h>  // Giroscópio
L3G gyro;
#endif  //GIROSCOPIO

/*---------------PARAQUEDAS---------------*/
#if USANDO_PARAQUEDAS

const long intervalIGN = 5000;  //Tempo que o ignitor fica ligado

#if USANDO_PARAQUEDAS_1
#define IGN_1 36                       /*act1*/
int paraquedas1armado = 0;             //Estado do paraquedas
unsigned long previousMillisPRQ1 = 0;  //Tempo de ativação
#endif                                 //PARAQUEDAS_1

#if USANDO_PARAQUEDAS_2
#define IGN_2 61                       /*act2*/
int paraquedas2armado = 0;             //Estado do paraquedas
unsigned long previousMillisPRQ2 = 0;  //Tempo de ativação
const long intervalPRQ2 = 2000;        //Intervalo do paraquedas
#endif                                 //PARAQUEDAS_2

#if USANDO_PARAQUEDAS_3
#define IGN_3 46                       /*act3*/
int paraquedas3armado = 0;             //Estado do paraquedas
unsigned long previousMillisPRQ3 = 0;  //Tempo de ativação
#endif                                 //PARAQUEDAS_3

#if USANDO_PARAQUEDAS_4
#define IGN_4 55                       /*act4*/
int paraquedas4armado = 0;             //Estado do paraquedas
unsigned long previousMillisPRQ4 = 0;  //Tempo de ativação
const long intervalPRQ4 = 2000;        //Intervalo do paraquedas
#endif                                 //PARAQUEDAS_4
#endif                                 //PARAQUEDAS

/*---------------SD DATALOGGER---------------*/
#if USANDO_SD
#include <SPI.h>  //Usado no DataLogger
#include <SD.h>   //Usado no DataLogger
//Declarações pro SD DataLogger
const int chipSelect = 53;
int lognumber = 0;
String nomearquivo;
String nomelog = "ZAMP";
String zerospacetext;
int zerospacelength;
#endif  //SD

/*---------------RELÓGIO---------------*/
#if USANDO_RELOGIO
float tempo = 0;  //Declaração pro relógio (millis)
#endif            //RELOGIO

void setup() {
/*---------------INICIALIZAÇÃO DATALOGGER---------------*/
#if USANDO_SD
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
#endif  //SD

/*---------------INICIALIZAÇÃO BMP---------------*/
#if USANDO_BMP
  //Inicialização do sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
//
#if USANDO_BMP_ALTURA
  //Reset de altura
  for (int i = 1; i <= 10; ++i) {
    tara += bmp.readAltitude();
  }
  tara /= 10;
#endif  //BMP_ALTURA
#endif  //BMP


/*---------------INICIALIZAÇÃO ACELERÔMETRO---------------*/
#if USANDO_ACELEROMETRO
  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_16_G);
#endif  //ACELEROMETRO

//Originalmente tinha um Wire.begin() aqui (entre o setup do acelerômetro e giroscópio), não sei de onde veio, e eu tirei.
//Se parar de rodar, tenta por de volta

/*---------------INICIALIZAÇÃO GIROSCÓPIO---------------*/
#if USANDO_GIROSCOPIO
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }
  gyro.enableDefault();
#endif  //GIROSCOPIO

/*---------------INICIALIZAÇÃO MAGNETÔMETRO---------------*/
#if USANDO_MAGNETOMETRO
  if (!mag.begin()) {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif  //MAGNETOMETRO

/*---------------PINOS DO PARAQUEDAS---------------*/
#if USANDO_PARAQUEDAS_1
  pinMode(IGN_1, OUTPUT);
#endif  //PARAQUEDAS_1

#if USANDO_PARAQUEDAS_2
  pinMode(IGN_2, OUTPUT);
#endif  //PARAQUEDAS_2

#if USANDO_PARAQUEDAS_3
  pinMode(IGN_3, OUTPUT);
#endif  //PARAQUEDAS_3

#if USANDO_PARAQUEDAS_4
  pinMode(IGN_4, OUTPUT);
#endif  //PARAQUEDAS_4

/*---------------SETUP ARMAZENAMENTO NO ARQUIVO---------------*/
#if USANDO_SD

  // Abre o arquivo
  File dataFile = SD.open(nomearquivo, FILE_WRITE);
  // Armazena o cabeçalho na dataString
  String dataString = "";

  //-----RELOGIO-----

#if USANDO_RELOGIO
  dataString += ("tempo\t");
#endif  //RELOGIO

  //-----BMP-----

#if USANDO_BMP_ALTURA
  dataString += ("Altura Sem Filtro\t");
  dataString += ("Altura Filtrada\t");
  dataString += ("Altura Filtrada 2\t");
  dataString += ("Detector de Queda\t");
#endif  //BMP_ALTURA

#if USANDO_BMP_TEMPERATURA
  dataString += ("Temperatura\t");
#endif  //BMP_TEMPERATURA

#if USANDO_BMP_PRESSAO
  dataString += ("Pressão\t");
#endif  //BMP_PRESSAO

  //-----PARAQUEDAS-----

#if USANDO_PARAQUEDAS_1
  dataString += ("paraquedas1armado\t");
#endif  //USANDO_PARAQUEDAS_1

#if USANDO_PARAQUEDAS_2
  dataString += ("paraquedas2armado\t");
#endif  //USANDO_PARAQUEDAS_2

#if USANDO_PARAQUEDAS_3
  dataString += ("paraquedas3armado\t");
#endif  //USANDO_PARAQUEDAS_3

#if USANDO_PARAQUEDAS_4
  dataString += ("paraquedas4armado\t");
#endif  //USANDO_PARAQUEDAS_4

  //-----ACELEROMETRO-----

#if USANDO_ACELEROMETRO_X
  dataString += ("x (m/s^2)\t");
#endif  //ACELEROMETRO_X

#if USANDO_ACELEROMETRO_Y
  dataString += ("y (m/s^2)\t");
#endif  //ACELEROMETRO_Y

#if USANDO_ACELEROMETRO_Z
  dataString += ("z (m/s^2)\t");
#endif  //ACELEROMETRO_Z

  //-----GIROSCOPIO-----

#if USANDO_GIROSCOPIO_X
  dataString += ("Gyro x\t");
#endif  //GIROSCOPIO_X

#if USANDO_GIROSCOPIO_Y
  dataString += ("Gyro y\t");
#endif  //GIROSCOPIO_Y

#if USANDO_GIROSCOPIO_Z
  dataString += ("Gyro z\t");
#endif  //GIROSCOPIO_Z

  //-----MAGNETOMETRO-----

#if USANDO_MAGNETOMETRO_X
  dataString += ("Magnet x\t");
#endif  //MAGNETOMETRO_X

#if USANDO_MAGNETOMETRO_Y
  dataString += ("Magnet y\t");
#endif  //MAGNETOMETRO_Y

#if USANDO_MAGNETOMETRO_Z
  dataString += ("Magnet z\t");
#endif  //MAGNETOMETRO_Z

  //-----DATASTRING----

  //Print da dataString
  Serial.println(dataString);

  /*---------------SALVA NO ARQUIVO---------------*/
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir" + nomearquivo);
  }
#endif  //SD
}

void loop() {

/*---------------COLETA DE DADOS---------------*/
#if USANDO_RELOGIO
  unsigned long currentMillis = millis();
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  tempo = currentMillis / 1000.0;
#endif

#if USANDO_ACELEROMETRO
  // Get a new sensor event (Acelerômetro)
  sensors_event_t event;
  accel.getEvent(&event);
#endif  //ACELEROMETRO

#if USANDO_GIROSCOPIO
  gyro.read();
#endif  //GIROSCOPIO

#if USANDO_MAGNETOMETRO
  // Get a new sensor event (Magnetômetro)
  sensors_event_t eventmag;
  mag.getEvent(&eventmag);
#endif  //MAGNETOMETRO

//*------SENSOR BMP------*/
#if USANDO_BMP_TEMPERATURA
  temperatura = bmp.readTemperature();
#endif  //Temperatura BMP

#if USANDO_BMP_PRESSAO
  pressao = bmp.readPressure();
#endif  //Pressao BMP

#if USANDO_BMP_ALTURA
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
#endif                               //Altura BMP


/*------ACIONAMENTO DE PARAQUEDAS------*/
#if USANDO_PARAQUEDAS_1
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
#endif  //Paraquedas 1

#if USANDO_PARAQUEDAS_2
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
#endif  //Paraquedas 2

#if USANDO_PARAQUEDAS_3
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
#endif  //Paraquedas 3

#if USANDO_PARAQUEDAS_4
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
#endif  //Paraquedas 4

/*------ARMAZENAMENTO DE DADOS------*/
#if USANDO_SD
  //Abre o arquivo
  File dataFile = SD.open(nomearquivo, FILE_WRITE);
  // Armazenamento dos valores na dataString
  String dataString = "";

//------------RELOGIO------------
#if USANDO_RELOGIO
  dataString += String(tempo) + "\t";
#endif  //RELOGIO

//------------BMP------------
#if USANDO_BMP_ALTURA
  dataString += String(vetor[guia]) + "\t";
  dataString += String(altura_filtrada) + "\t";
  dataString += String(altura_filtrada2) + "\t";
  dataString += String(detectorqueda) + "\t";
#endif  //Altura

#if USANDO_BMP_TEMPERATURA
  dataString += String(temperatura) + "\t";
#endif  //Temperatura

#if USANDO_BMP_PRESSAO
  dataString += String(pressao) + "\t";
#endif  //Pressao

//------------PARAQUEDAS------------
#if USANDO_PARAQUEDAS_1
  dataString += String(paraquedas1armado) + "\t";
#endif  //PARAQUEDAS 1

#if USANDO_PARAQUEDAS_2
  dataString += String(paraquedas2armado) + "\t";
#endif  //PARAQUEDAS 2

#if USANDO_PARAQUEDAS_3
  dataString += String(paraquedas3armado) + "\t";
#endif  //PARAQUEDAS 3

#if USANDO_PARAQUEDAS_4
  dataString += String(paraquedas4armado) + "\t";
#endif  //PARAQUEDAS 4

//------------ACELEROMETRO------------
#if USANDO_ACELEROMETRO_X
  dataString += String(event.acceleration.x) + "\t";
#endif  //ACELEROMETRO X

#if USANDO_ACELEROMETRO_Y
  dataString += String(event.acceleration.y) + "\t";
#endif  //ACELEROMETRO Y

#if USANDO_ACELEROMETRO_Z
  dataString += String(event.acceleration.z) + "\t";
#endif  //ACELEROMETRO Z

//------------GIROSCOPIO------------
#if USANDO_GIROSCOPIO_X
  dataString += String((int)gyro.g.x) + "\t";
  #endif  //GIROSCOPIO X

  #if USANDO_GIROSCOPIO_Y
  dataString += String((int)gyro.g.y) + "\t";
  #endif  //GIROSCOPIO Y

  #if USANDO_GIROSCOPIO_Z
  dataString += String((int)gyro.g.z) + "\t";
  #endif  //GIROSCOPIO Y

//------------MAGNETOMETRO------------
#if USANDO_MAGNETOMETRO_X
  dataString += String(eventmag.magnetic.x) + "\t";
  #endif  //MAGNETOMETRO X

  #if USANDO_MAGNETOMETRO_Y
  dataString += String(eventmag.magnetic.y) + "\t";
  #endif  //MAGNETOMETRO Y

  #if USANDO_MAGNETOMETRO_Z
  dataString += String(eventmag.magnetic.z) + "\t";
  #endif  //MAGNETOMETRO Z

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
#endif  //SD
}
