#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/


Adafruit_BMP085 bmp;

//Definindo SD
#define chipSelect 53
String nomeBaseSD = "data";
String nomeSD;

//Definindo variaveis filtros
float var;
float alturaInicial;
#define numLeituras 20
float leituras[numLeituras];
float medias[numLeituras];
int indiceLeitura = 0;
int indiceMedia = 0;
float soma = 0;
float somaMedias = 0;
float media = 0;
float mediaDasMedias = 0;

//Definindo variaveis apogeu
#define historicoTamanho 20
float historico[historicoTamanho];
int indiceHistorico = 0;
int contadorHistorico = 0;

//Definindo variaveis paraquedas
int intervaloTempo = 10000;
int intervaloDelay = 5000;
bool paraquedas1 = false;
bool paraquedas1data = false;
unsigned long tempoP1 = 0;
bool paraquedas2 = false;
bool paraquedas2data = false;
unsigned long tempoP2 = 0;
bool paraquedas3 = false;
bool paraquedas3data = false;
unsigned long tempoP3 = 0;
bool paraquedas4 = false;
bool paraquedas4data = false;
unsigned long tempoP4 = 0;


void setup() {
  Serial.begin(115200);

  //Inicializando BMP e SD
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }

  //Definindo pinos dos paraquedas
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);


  //Primeiras leituras BMP
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

  //Inicializando vetor de historico
  for (int i = 0; i < historicoTamanho; i++) {
    historico[i] = 0;
  }

  //Definindo cabecalho
  String dataStringInicial = "Temperature(*C)\tPressure(Pa)\tAltitude com primeiro filtro(m)\tAltitude com segundo filtro(m)\tAltitude sem filtro(m)\tStatus\tParaquedas 1\tParaquedas 2\tParaquedas 3\tParaquedas 4\n";
  Serial.println(dataStringInicial);

  //Logica para nome do arquivo SD
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
  //Inicializando a string
  String dataString = "";

  //Filtro 1
  soma -= leituras[indiceLeitura];
  var = bmp.readAltitude() - alturaInicial;
  leituras[indiceLeitura] = var;
  soma += leituras[indiceLeitura];
  if (++indiceLeitura >= numLeituras) {
    indiceLeitura = 0;
  }

  media = soma / numLeituras;

  //Filtro 2
  somaMedias -= medias[indiceMedia];
  medias[indiceMedia] = media;
  somaMedias += medias[indiceMedia];
  if (++indiceMedia >= numLeituras) {
    indiceMedia = 0;
  }

  mediaDasMedias = somaMedias / numLeituras;

  //Apogeu
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

  //Paraquedas 1
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

  //Paraquedas 2
  if (estaDescendo && !paraquedas2) {
    paraquedas2 = true;
    tempoP2 = millis();
  }
  if (paraquedas2 && currentTime >= tempoP2 + intervaloDelay && currentTime < tempoP2 + intervaloDelay + intervaloTempo) {
    paraquedas2data = true;
    digitalWrite(IGN_2, HIGH);
  } else if (paraquedas2 && currentTime >= tempoP2 + intervaloDelay + intervaloTempo) {
    paraquedas2data = false;
    digitalWrite(IGN_2, LOW);
  }

  //Paraquedas 3
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

  //Paraquedas 4
  if (estaDescendo && !paraquedas4) {
    paraquedas4 = true;
  }
  if (paraquedas4 && mediaDasMedias <= -3 && tempoP4 == 0) {
    tempoP4 = millis();
  } 
  if (paraquedas4 && tempoP4 != 0 && currentTime >= tempoP4 + intervaloDelay && currentTime < tempoP4 + intervaloDelay + intervaloTempo) {
    paraquedas4data = true;
    digitalWrite(IGN_4, HIGH);
  } else if (paraquedas4 && currentTime >= tempoP4 + intervaloDelay + intervaloTempo) {
    paraquedas4data = false;
    digitalWrite(IGN_4, LOW);
  }

  //String de dados
  dataString += String(bmp.readTemperature());
  dataString += "\t";
  dataString += String(bmp.readPressure());
  dataString += "\t";
  dataString += String(media);
  dataString += "\t";
  dataString += String(mediaDasMedias);
  dataString += "\t";
  dataString += String(var);
  dataString += "\t";

  dataString += String(estaDescendo);
  dataString += "\t";
  dataString += String(paraquedas1data);
  dataString += "\t";
  dataString += String(paraquedas2data);
  dataString += "\t";
  dataString += String(paraquedas3data);
  dataString += "\t";
  dataString += String(paraquedas4data);
  dataString += "\t";
  
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
