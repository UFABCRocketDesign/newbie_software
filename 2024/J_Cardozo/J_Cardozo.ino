#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;

const int chipSelect = 53;

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


void setup() {
  Serial.begin(115200);

  Serial.print("Inicializando BMP...");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }

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

  String dataStringInicial = "Temperature(*C)\tPressure(Pa)\tAltitude com primeiro filtro(m)\tAltitude com segundo filtro(m)\tAltitude sem filtro(m)\tStatus\n";
  Serial.println(dataStringInicial);

  String nomeSD = "datalog";
  int iSD = 0;
  while (true) {
    nomeSD += iSD;
    if (SD.exists(nomeSD)) {
      Serial.println("ja existe um arquivo com esse nome.");
      nomeSD -= iSD;
    } else {
      File dataFile = SD.open(nomeSD, FILE_WRITE);
      break;
    }
    i++;
  }

  if (dataFile) {
    dataFile.println(dataStringInicial);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt");
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
    if (historico[(indiceHistorico + i - 1) % historicoTamanho] >= historico[(indiceHistorico + i) % historicoTamanho]) {
      contadorHistorico++;
    }
  }

  bool estaDescendo = false;

  if (contadorHistorico >= 0.7 * historicoTamanho) {
    estaDescendo = true;
  }
  contadorHistorico = 0;

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
  Serial.println(dataString);

  File dataFile = SD.open(nomeSD, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt");
  }
}
