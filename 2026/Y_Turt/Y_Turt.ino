#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
// --- Variáveis para ajuste de altura ---
float altitudeInicial; //definiçao da altura inicial
float altitudeAprox[10]; //definição da altura suave
float altitudeFiltrado[10]; //definição da altura suave 2
// --- Variáveis para detecção de queda ---
float altitudeMaxima = 0.0;
bool emQueda = false;
const float margemQueda = 1.5;
int contadorQueda = 0;                 
const int confirmacoesNecessarias = 5; 

// --- Constante SD
#include <SPI.h>
#include <SD.h>
const int chipSelect = 53;


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
    while (true);
  }
Serial.println("initialization done.");


// Calibração do referencial zero
float soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma/10; //definição da altura inicial
  
  Serial.println("*C\tPa\tmeters\tPa\tmeters");
}

void loop() {
 float Temperatura = bmp.readTemperature();
 float Pressao = bmp.readPressure();
  Serial.print(Temperatura);
  Serial.print("\t");
  Serial.print(Pressao);
  Serial.print("\t");



    //definição da altura = 0
    float altura = bmp.readAltitude() - altitudeInicial; 
    Serial.print(altura);
    Serial.print("\t"); 

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
  Serial.print(altitudeSuave);
  Serial.print("\t");
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
  Serial.print(altitudeFinal);
  Serial.print("\t");
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
Serial.print(contadorQueda);
Serial.print("\t");
  Serial.print(emQueda); 
  




// sd
String dataString = "";

  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(Temperatura) + "/t";
    dataString += String(Pressao) + "/t";
    dataString += String(altura) + "/t";
    dataString += String(altitudeSuave) + "/t";
    dataString += String(contadorQueda) + "/t";
    dataString += String(emQueda) + "/t";
  }

// --- Salvar Dados no SD ---
File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);  
  }
  else {
    Serial.println("error opening datalog.txt");
  }


  Serial.print("\t");
  Serial.println();
}