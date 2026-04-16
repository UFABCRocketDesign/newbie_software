#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
// --- Variáveis para ajuste de altura ---
float altitudeInicial;     //definiçao da altura inicial
float alturaAprox[10];     //definição da altura suave
float alturaFiltrado[10];  //definição da altura suave 2
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
const long interval1 = 10000;
unsigned long previousMillis1 = 0;
// int pin1State = LOW;
// bool pin1Iniciado = false;
// bool pin1Concluido = false;
int pin1Estado = 0;


const long interval2 = 10000;
const long interTimer = 2000;
unsigned long previousMillis2 = 0;
// int pin2State = LOW;
// bool pin2Iniciado = false;
// bool pin2Concluido = false;
int pin2Estado = 0;

const long interval3 = 10000;
unsigned long previousMillis3 = 0;
int pin3Estado = 0;





void setup() {

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
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
}

void loop() {

  float Temperatura = bmp.readTemperature();
  float Pressao = bmp.readPressure();

  //definição da altura = 0
  float altura = bmp.readAltitude() - altitudeInicial;


  // --- Primeiro Filtro (Média Móvel Simples) ---
  for (int i = 0; i < 9; i++) {
    alturaAprox[i] = alturaAprox[i + 1];
  }
  alturaAprox[9] = altura;
  float somaAprox = 0;
  for (int i = 0; i < 10; i++) {
    somaAprox += alturaAprox[i];
  }
  float alturaSuave = somaAprox / 10;

  // --- Segundo Filtro (Cascata) ---
  for (int i = 0; i < 9; i++) {
    alturaFiltrado[i] = alturaFiltrado[i + 1];
  }
  alturaFiltrado[9] = alturaSuave;
  float somaFiltrado = 0;
  for (int i = 0; i < 10; i++) {
    somaFiltrado += alturaFiltrado[i];
  }
  float alturaFinal = somaFiltrado / 10;


  // --- LÓGICA DE DETECÇÃO DE QUEDA COM MÚLTIPLAS CONFIRMAÇÕES ---
  if (alturaFinal > altitudeMaxima) {
    altitudeMaxima = alturaFinal;
  }
  if (!emQueda && alturaFinal < (altitudeMaxima - margemQueda)) {
    contadorQueda += 1;
    if (contadorQueda >= confirmacoesNecessarias) {
      emQueda = true;
    }
  }



  unsigned long currentMillis1 = millis();
  if (emQueda && pin1Estado != 2) {
    // if the LED is off turn it on and vice-versa:
    if (pin1Estado == 0) {
      // pin1State = HIGH;
      pin1Estado = 1;
      previousMillis1 = currentMillis1;  //salva quando ele ligou

    } else if ((currentMillis1 - previousMillis1 >= interval1) && (pin1Estado == 1)) {
      //pin1State = LOW;
      pin1Estado = 2;
    }

    // set the LED with the ledState of the variable:

    digitalWrite(IGN_1, pin1Estado == 1);
  }



  unsigned long currentMillis2 = millis();
  if (emQueda && pin2Estado != 3) {
    // if the LED is off turn it on and vice-versa:
    if (pin2Estado != 2) {
      if (pin2Estado == 0) {
        // pin2State = HIGH;
        pin2Estado = 1;
        previousMillis2 = currentMillis2;  // salva quando q ligou

      } else if (currentMillis2 - previousMillis2 >= interTimer && pin2Estado == 1) {
        // pin2State = LOW;
        pin2Estado = 2;
        previousMillis2 = currentMillis2;
      }
    } else if (currentMillis2 - previousMillis2 >= interval2 && pin2Estado == 2) {
      pin2Estado = 3;
    }
    digitalWrite(IGN_2, pin2Estado == 2);
  }



  unsigned long currentMillis3 = millis();
  if (emQueda && pin3Estado != 2 && alturaFinal < -2) {
    // if the LED is off turn it on and vice-versa:
    if (pin3Estado == 0) {
      // pin1State = HIGH;
      pin3Estado = 1;
      previousMillis3 = currentMillis3;  //salva quando ele ligou

    } else if ((currentMillis3 - previousMillis3 >= interval3) && (pin3Estado == 1)) {
      //pin1State = LOW;
      pin3Estado = 2;
    }

    // set the LED with the ledState of the variable:

    digitalWrite(IGN_3, pin3Estado == 1);
  }


  // sd
  String dataString = "";
  dataString += String(Temperatura) + "\t";
  dataString += String(Pressao) + "\t";
  dataString += String(altura) + "\t";
  dataString += String(alturaSuave) + "\t";
  dataString += String(alturaFinal) + "\t";
  dataString += String(contadorQueda) + "\t";
  dataString += String(emQueda) + "\t";
  dataString += String(pin1Estado) + "\t";
  dataString += String(pin2Estado) + "\t";
  dataString += String(pin3Estado) + "\t";

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