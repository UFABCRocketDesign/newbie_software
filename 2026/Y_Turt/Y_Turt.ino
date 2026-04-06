#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
// --- Variáveis para ajuste de altura ---
float altitudeInicial;       //definiçao da altura inicial
float altitudeAprox[10];     //definição da altura suave
float altitudeFiltrado[10];  //definição da altura suave 2
// --- Variáveis para detecção de queda ---
float altitudeMaxima = 0.0;
bool emQueda = false;
const float margemQueda = 1.5;
int contadorQueda = 0;
const int confirmacoesNecessarias = 5;
//Nome do Arquivo SD
String baseNome = "YBT";
String nomeFile = "";
// --- Constante SD
#include <SPI.h>
#include <SD.h>
const int chipSelect = 53;


#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/
const long interval = 1000;
unsigned long previousMillis = 0;
int pinState = LOW;
bool pin1Iniciado = false;
bool pin1Concluido = false;



  void
  setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  // -- Inicialização do SD
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

  // Calibração do referencial zero
  float soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma / 10;  //definição da altura inicial

  //Definicao arquivo SD
  int i = 0;
  do {
    i++;
    String numeroNome = String(i);
    int numeroZeros = 8 - (numeroNome.length() + baseNome.length());
    String zerosNome = "";
    for (int j = 0; j < numeroZeros; j++) {
      zerosNome += "0";
    }
    nomeFile = baseNome + zerosNome + numeroNome + ".txt";
  } while (SD.exists(nomeFile));
  Serial.println(nomeFile);

  //Cabecalho SD
  String cabecalhoString = "";
  cabecalhoString += "*C\tPa\tmeters\tPa\tmeters\tmeters\tmeters\t(0-5)\t(0/1)";
  Serial.println(cabecalhoString);

  File dataFile = SD.open(nomeFile, FILE_WRITE);
  if (dataFile) {
    dataFile.println(cabecalhoString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
  pinMode(IGN_1, OUTPUT);
}

void loop() {

  float Temperatura = bmp.readTemperature();
  float Pressao = bmp.readPressure();

  //definição da altura = 0
  float altura = bmp.readAltitude() - altitudeInicial;


  // --- Primeiro Filtro (Média Móvel Simples) ---
  for (int i = 0; i < 9; i++) {
    altitudeAprox[i] = altitudeAprox[i + 1];
  }
  altitudeAprox[9] = altura;
  float somaAprox = 0;
  for (int i = 0; i < 10; i++) {
    somaAprox += altitudeAprox[i];
  }
  float altitudeSuave = somaAprox / 10;

  // --- Segundo Filtro (Cascata) ---
  for (int i = 0; i < 9; i++) {
    altitudeFiltrado[i] = altitudeFiltrado[i + 1];
  }
  altitudeFiltrado[9] = altitudeSuave;
  float somaFiltrado = 0;
  for (int i = 0; i < 10; i++) {
    somaFiltrado += altitudeFiltrado[i];
  }
  float altitudeFinal = somaFiltrado / 10;


  // --- LÓGICA DE DETECÇÃO DE QUEDA COM MÚLTIPLAS CONFIRMAÇÕES ---
  if (altitudeFinal > altitudeMaxima) {
    altitudeMaxima = altitudeFinal;
  }
  if (!emQueda && altitudeFinal < (altitudeMaxima - margemQueda)) {
    contadorQueda += 1;
    if (contadorQueda >= confirmacoesNecessarias) {
      emQueda = true;
    }
  }

  unsigned long currentMillis = millis();
  if (emQueda && !pin1Concluido) {
      // if the LED is off turn it on and vice-versa:
    if (pinState == LOW && !pin1Iniciado){
        pinState = HIGH;
         pin1Iniciado = true;
         previousMillis = currentMillis;
      }
    else {
      pinState = LOW;
      if (currentMillis - previousMillis >= interval) {
        pin1Concluido = true;
      }

      // set the LED with the ledState of the variable:
      digitalWrite(IGN_1, pinState);
    }
  }

  // sd
  String dataString = "";
  dataString += String(Temperatura) + "\t";
  dataString += String(Pressao) + "\t";
  dataString += String(altura) + "\t";
  dataString += String(altitudeSuave) + "\t";
  dataString += String(contadorQueda) + "\t";
  dataString += String(emQueda) + "\t";
  dataString += String(pinState) + "\t";

  // --- Salvar Dados no SD ---
  File dataFile = SD.open(nomeFile, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }

  Serial.println(dataString);
}