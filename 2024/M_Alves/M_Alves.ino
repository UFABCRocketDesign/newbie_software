#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 53;

int fileNum = 0;
int qtdSaves = 9999;
String qtdZeros;
String sdName = "Math";
String fileName;

Adafruit_BMP085 bmp;

// Altitude Inicial
float AltInicial = 0;
int numLeiturasInicial = 25;
float somaAltInicial = 0;

// Variáveis para a média simples
const int numLeituras = 15;   // número de leituras para a média
float leituras[numLeituras];  // as leituras do sensor de altitude
int indiceLeitura = 0;        // o índice da leitura atual
float somaLeituras = 0;       // a soma das leituras
float mediaAltitude = 0;      // a média das leituras

// Variáveis para o segundo filtro
float leiturasFiltradas[numLeituras];  // as leituras filtradas
float somaLeiturasFiltradas = 0;       // a soma das leituras filtradas
float mediaAltitudeFiltrada = 0;       // a média das leituras filtradas

//Apogeu
float altitudeAnterior = -1;
int contador = 0;
int estado = 0;  // estado 0 -> subindo; estado 1 -> descendo

void setup() {
  //BME085
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  //SDCard
  if (!SD.begin(chipSelect)) {  // see if the card is present and can be initialized:
    Serial.println("Card failed, or not present");
  }

  //Leituras iniciais
  for (int i = 0; i < numLeiturasInicial; i++) {
    somaAltInicial += bmp.readAltitude();
  }

  AltInicial = somaAltInicial / numLeiturasInicial;  //Médias das leituras iniciais

  //Armazena x leituras para realizar a média
  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - AltInicial;
    somaLeituras += leituras[i];
    leiturasFiltradas[i] = leituras[i];
    somaLeiturasFiltradas += leiturasFiltradas[i];
  }

  //Cabeçalho
  String dadosString = "";
  dadosString += "Temperature (*C)\tPressure (Pa)\tRaw Altitude (m)\tFirst Filter (m)\tSecond Filter (m)\tEstado (0 ou 1)";
  Serial.println("Temperature (*C)\tPressure (Pa)\tRaw Altitude (m)\tFirst Filter (m)\tSecond Filter (m)\tEstado (0 ou 1) ");

  // Verifica se o arquivo existe e cria um novo se necessário
  for (int fileNum = 0; fileNum <= qtdSaves; fileNum++) {
    if (fileNum < 10) {
      qtdZeros = "000";
    } else if (fileNum < 100) {
      qtdZeros = "00";
    } else if (fileNum < 1000) {
      qtdZeros = "0";
    } else {
      qtdZeros = "";
    }
    fileName = sdName + qtdZeros + String(fileNum) + ".txt";
    if (!SD.exists(fileName)) {
      break;
    }
  }

  Serial.println(fileName);

  File dadosFile = SD.open(fileName, FILE_WRITE);
  if (dadosFile) {
    dadosFile.println(dadosString);
    dadosFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening");
  }
}

// the loop function runs over and over again forever
void loop() {
  String dadosString = "";

  //BME085
  dadosString += String(bmp.readTemperature()) + "\t";
  dadosString += String(bmp.readPressure()) + "\t";

  float rawAltitude = bmp.readAltitude() - AltInicial;
  dadosString += String(rawAltitude) + "\t";  //Altura do sensor sem filtro, rawww

  // Primeiro Filtro
  somaLeituras = somaLeituras - leituras[indiceLeitura];
  leituras[indiceLeitura] = rawAltitude;
  somaLeituras = somaLeituras + leituras[indiceLeitura];
  mediaAltitude = somaLeituras / numLeituras;
  dadosString += String(mediaAltitude) + "\t";

  // Segundo Filtro
  somaLeiturasFiltradas = somaLeiturasFiltradas - leiturasFiltradas[indiceLeitura];
  leiturasFiltradas[indiceLeitura] = mediaAltitude;  // usa a média do primeiro filtro
  somaLeiturasFiltradas = somaLeiturasFiltradas + leiturasFiltradas[indiceLeitura];
  mediaAltitudeFiltrada = somaLeiturasFiltradas / numLeituras;
  dadosString += String(mediaAltitudeFiltrada) + "\t";

  indiceLeitura++;
  if (indiceLeitura >= numLeituras) {  //se for o último vetor, volta para o início
    indiceLeitura = 0;
  }

  // Apogeu
  if (altitudeAnterior != -1 && mediaAltitudeFiltrada < altitudeAnterior) {
    contador++;
    if (contador >= 15) {
      estado = 1;
    }
  } else {
    contador = 0;
    estado = 0;
  }

  dadosString += String(estado);
  altitudeAnterior = mediaAltitudeFiltrada;  // Atualize a altitude anterior para a próxima iteração

  File dadosFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dadosFile) {
    dadosFile.println(dadosString);
    dadosFile.close();
    // print to the serial port too:
    Serial.println(dadosString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening");
  }
}