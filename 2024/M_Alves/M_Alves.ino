#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

//Pinos do paraquedas
#define IGN_1 36         /*act1*/
bool ativacao1 = false;  //variável para garantir que só vai ativar 1 vez o pino do paraquedas 1

#define IGN_2 61 /*act2*/
bool ativacao2 = false; //variável para garantir que só vai ativar 1 vez o pino do paraquedas 2

#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/

//Relógio interno do arduino p/ o paraquedas 1 e paraquedas 2
unsigned long futureMillis = 0;  
const long interval = 10000;     
unsigned long futureMillis2 = 0;
const long interval2 = 5000;

const int chipSelect = 53;

//SDCard
int fileNum = 0;
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

  //Paraquedas 1 e 2
  pinMode(IGN_1, OUTPUT);
  digitalWrite(IGN_1, LOW);
  pinMode(IGN_2, OUTPUT);
  digitalWrite(IGN_2, LOW);

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
  dadosString += "Temperature (*C)\tPressure (Pa)\tRaw Altitude (m)\tFirst Filter (m)\tSecond Filter (m)\tEstado (0 ou 1)\tParaquedas1 (bool)\tParaquedas2 (bool)\n";
  Serial.println("Temperature (*C)\tPressure (Pa)\tRaw Altitude (m)\tFirst Filter (m)\tSecond Filter (m)\tEstado (0 ou 1)\tParaquedas1 (bool)\tParaquedas2 (bool)");

  // Verifica se o arquivo existe e cria um novo se necessário
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

  // Apogeu + millis
  unsigned long currentMillis = millis();

  if (altitudeAnterior != -1 && mediaAltitudeFiltrada < altitudeAnterior) {
    contador++;
    if (contador >= 25) {
      estado = 1;
    }
  } else {
    contador = 0;
    estado = 0;
  }

  if (estado == 1 && ativacao1 == false) {
    futureMillis = currentMillis + interval;
    futureMillis2 = currentMillis + interval2;
    digitalWrite(IGN_1, HIGH);
    ativacao1 = true;
    ativacao2 = true;
  }

  if(ativacao2 == true && currentMillis >= futureMillis2) {
    digitalWrite(IGN_2, HIGH);
    ativacao2 = false;
    futureMillis2 = currentMillis + interval;
  }

  if (currentMillis >= futureMillis) {
    digitalWrite(IGN_1, LOW);
  }

  if (currentMillis >= futureMillis2) {
    digitalWrite(IGN_2, LOW);
  }

  dadosString += String(estado) + "\t";
  dadosString += String(digitalRead(IGN_1)) + "\t";
  dadosString += String(digitalRead(IGN_2));

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