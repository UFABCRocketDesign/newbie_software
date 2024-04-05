#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/


Adafruit_BMP085 bmp;

const int chipSelect = 53;
String nomeBaseSD = "data";
String nomeSD;

float var;
float alturaInicial;
const int numLeituras = 20;
float leituras[numLeituras];
float medias[numLeituras];
int indiceLeitura = 0;
int indiceMedia = 0;
float soma = 0;
float somaMedias = 0;
float media = 0;
float mediaDasMedias = 0;

const int historicoTamanho = 20;
float historico[historicoTamanho];
int indiceHistorico = 0;
int contadorHistorico = 0;

int intervaloTempo = 10000;
bool paraquedas1 = false;
bool paraquedas1data = false;
unsigned long tempoP1 = 0;
bool paraquedas2 = false;
bool paraquedas2data = false;
unsigned long tempoP2 = 0;
bool paraquedas3 = false;
bool paraquedas3data = false;
unsigned long tempoP3 = 0;


void setup() {
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }

  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);


  for (int i = 0; i < numLeituras; i++) {
    soma += bmp.readAltitude();
  }

  alturaInicial = soma / numLeituras;
  soma = 0;

  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - alturaInicial;
    soma += leituras[i];
    medias[i] = 0;
  }

  for (int i = 0; i < historicoTamanho; i++) {
    historico[i] = 0;
  }

  String dataStringInicial = "Temperature(*C)\tPressure(Pa)\tAltitude com primeiro filtro(m)\tAltitude com segundo filtro(m)\tAltitude sem filtro(m)\tStatus\tParaquedas 1\tParaquedas 2\tParaquedas 3\n";
  Serial.println(dataStringInicial);

  int iSD = 0;
  while (true) {
    int numZeros = 8 - nomeBaseSD.length() - String(iSD).length();
    String zeros = "";
    for (int i = 0; i < numZeros; i++) {
      zeros += "0";
    }

    nomeSD = nomeBaseSD + zeros + String(iSD) + ".txt";

    Serial.println("Verificando: " + nomeSD);

    if (SD.exists(nomeSD)) {
      Serial.println(nomeSD + " já existe.");
    } else {
      Serial.println(nomeSD + " não existe, criando...");
      File dataFile = SD.open(nomeSD, FILE_WRITE);
      if (dataFile) {
        Serial.println(nomeSD + " criado.");
        dataFile.println(dataStringInicial);
        dataFile.close();
      } else {
        Serial.println("Erro ao abrir " + nomeSD);
      }
      break;
    }
    iSD++;
  }
}

void loop() {
  String dataString = "";

  soma -= leituras[indiceLeitura];
  var = bmp.readAltitude() - alturaInicial;
  leituras[indiceLeitura] = var;
  soma += leituras[indiceLeitura];
  if (++indiceLeitura >= numLeituras) {
    indiceLeitura = 0;
  }

  media = soma / numLeituras;

  somaMedias -= medias[indiceMedia];
  medias[indiceMedia] = media;
  somaMedias += medias[indiceMedia];
  if (++indiceMedia >= numLeituras) {
    indiceMedia = 0;
  }

  mediaDasMedias = somaMedias / numLeituras;

  historico[indiceHistorico] = mediaDasMedias;
  if (++indiceHistorico >= historicoTamanho) {
    indiceHistorico = 0;
  }

  for (int i = 1; i < historicoTamanho; i++) {
    if (historico[(indiceHistorico + i - 1) % historicoTamanho] > historico[(indiceHistorico + i) % historicoTamanho]) {
      contadorHistorico++;
    }
  }

  bool estaDescendo = false;

  if (contadorHistorico >= 0.7 * historicoTamanho) {
    estaDescendo = true;
  }
  contadorHistorico = 0;

  unsigned long currentTime = millis();
  if (estaDescendo && !paraquedas1) {
    paraquedas1 = true;
    tempoP1 = millis();
    paraquedas1data = true;
    digitalWrite(IGN_1, HIGH);
  }
  if (paraquedas1 && currentTime >= tempoP1 + intervaloTempo) {
    paraquedas1data = false;
    digitalWrite(IGN_1, LOW);
  }

  if (estaDescendo && !paraquedas2) {
    paraquedas2 = true;
    tempoP2 = millis();
  }
  if (paraquedas2 && currentTime >= tempoP2 + intervaloTempo && currentTime < tempoP2 + 2*intervaloTempo) {
    paraquedas2data = true;
    digitalWrite(IGN_2, HIGH);
  } else if (paraquedas2 && currentTime >= tempoP2 + 2*intervaloTempo) {
    paraquedas2data = false;
    digitalWrite(IGN_2, LOW);
  }

  if (estaDescendo && !paraquedas3) {
    paraquedas3 = true;
  }
  if (paraquedas3 && mediaDasMedias <= -3 && tempoP3 == 0) {
    tempoP3 = millis();
    paraquedas3data = true;
    digitalWrite(IGN_3, HIGH);
  } 
  if (paraquedas3 && tempoP3 != 0 && currentTime >= tempoP3 + intervaloTempo) {
    paraquedas3data = false;
    digitalWrite(IGN_3, LOW);
  }

  dataString += bmp.readTemperature();
  dataString += "\t";
  dataString += bmp.readPressure();
  dataString += "\t";
  dataString += media;
  dataString += "\t";
  dataString += mediaDasMedias;
  dataString += "\t";
  dataString += var;
  dataString += "\t";

  if (estaDescendo) {
    dataString += "1";
    dataString += "\t";
  } else {
    dataString += "0";
    dataString += "\t";
  }

  if (paraquedas1data) {
    dataString += "1";
    dataString += "\t";
  } else {
    dataString += "0";
    dataString += "\t";
  }

  if (paraquedas2data) {
    dataString += "1";
    dataString += "\t";
  } else {
    dataString += "0";
    dataString += "\t";
  }

  if (paraquedas3data) {
    dataString += "1";
    dataString += "\t";
  } else {
    dataString += "0";
    dataString += "\t";
  }
  Serial.println(dataString);

  File dataFile = SD.open(nomeSD, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.print("Error opening ");
    Serial.print(nomeSD);
    Serial.println();
  }
}
