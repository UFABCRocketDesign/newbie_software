#include <SD.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif           // ARDUINO_AVR_MEGA2560
#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/
int chipSelect = 53;
float altitudeInicial;
float somaAltitude;
float listaSuavizarCurva_0[10];
float listaSuavizarCurva_1[5];
float listaDeteccaoQueda[2];
int contador = 0;
float alturaAnterior;
int numA = 0;
File arquivo;
String nomeArquivo;
String inicio = "isa";
String tipoDeArquivo = ".txt";
String header = "Temperature(C)\tPressure(Pa)\tHigh(meters)\tFiltered High 0(meters)\tFiltered High 1(meters)\tFallen(1)/ Not fallen (0)\tContador de Queda\tEstadoP1\tEstadoP2\tEstadoP3\tEstadoP4";
float timerP1;
float timerP2;
float timerP3;
float timerP4;
int intervaloParaquedas = 5000;
int atrasoP1_P2 = 2000;
int atrasoP1_P4 = 2000;
int estadoP1 = 0;  // estado de piscar
int estadoP2 = 0;
int estadoP3 = 0;
int estadoP4 = 0;


float filtroSuavizarCurva_0(float dadosCurva_0) {

  for (int i = 9; i > 0; i--) {
    listaSuavizarCurva_0[i] = listaSuavizarCurva_0[i - 1];
  }
  listaSuavizarCurva_0[0] = dadosCurva_0;
  float somaLista = 0;
  for (int i = 0; i < 10; i++) {
    somaLista += listaSuavizarCurva_0[i];
  }
  float mediaFiltro = somaLista / 10;
  return mediaFiltro;
}

float filtroSuavizarCurva_1(float dadosCurva_1) {

  for (int i = 4; i > 0; i--) {
    listaSuavizarCurva_1[i] = listaSuavizarCurva_1[i - 1];
  }
  listaSuavizarCurva_1[0] = dadosCurva_1;
  float somaLista = 0;
  for (int i = 0; i < 5; i++) {
    somaLista += listaSuavizarCurva_1[i];
  }
  float mediaFiltro = somaLista / 5;
  return mediaFiltro;
}

void setup() {

  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Não foi possível achar um sensor BMP085 válido, verifique os fios!");
    while (1) {}
  }

  //abrindo o SD
  pinMode(chipSelect, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("Falha ao inicializar o cartão SD");
    return;
  }
  Serial.println("Cartão SD inicializado com sucesso");

  //verificando nome de arquivo
  do {
    int tamArquivo = inicio.length() + String(numA).length();
    String zerosAdicionais = "";
    for (int i = tamArquivo; i < 8; i++) {
      zerosAdicionais += "0";
    }
    nomeArquivo = inicio + zerosAdicionais + String(numA) + tipoDeArquivo;
    numA++;
  } while (SD.exists(nomeArquivo));

  Serial.println(nomeArquivo);
  arquivo = SD.open(nomeArquivo, FILE_WRITE);
  if (arquivo) {

    arquivo.println(header);
    arquivo.close();
  } else {
    Serial.println("Erro ao abrir o arquivo");
  }

  Serial.println(header);

  //determinando altitude inicial
  somaAltitude = 0;
  for (int posicaoListaAltitude = 0; posicaoListaAltitude < 10; posicaoListaAltitude++) {
    somaAltitude += bmp.readAltitude();
  }
  altitudeInicial = somaAltitude / 10;

  //definindo pino como porta de saída
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
}

void loop() {
  //determinando o apogeu
  float altura = bmp.readAltitude() - altitudeInicial;
  float alturaFiltrada_0 = filtroSuavizarCurva_0(altura);
  float alturaFiltrada_1 = filtroSuavizarCurva_1(alturaFiltrada_0);
  int fallenCondition = 0;

  if (alturaFiltrada_1 < alturaAnterior) {
    contador++;
  } else {
    contador = 0;
  }
  alturaAnterior = alturaFiltrada_1;
  fallenCondition = (contador >= 5) ? 1 : 0;

  //acionando o primeiro paraquedas
  bool queda = (fallenCondition == 1);

  if (queda && estadoP1 == 0) {  //verifica se esta caindo e se o led não esta piscando
    estadoP1 = 1;
    digitalWrite(IGN_1, HIGH);
    timerP1 = millis() + intervaloParaquedas;
  }
  if (estadoP1 == 1 && (millis() >= timerP1)) {
    estadoP1 = 2;
    digitalWrite(IGN_1, LOW);
  }

  //acionando segundo paraquedas
  if (queda && estadoP2 == 0) {
    timerP2 = millis() + atrasoP1_P2;
    estadoP2 = 1;
  }
  if (estadoP2 == 1 && (millis() >= timerP2)) {
    estadoP2 = 2;
    digitalWrite(IGN_2, HIGH);
    timerP2 = millis() + intervaloParaquedas;
  }
  if (estadoP2 == 2 && (millis() >= timerP2)) {
    estadoP2 = 3;
    digitalWrite(IGN_2, LOW);
  }

  //acionando terceiro paraquedas
  if (queda && estadoP3 == 0 && alturaFiltrada_1 < -3) {  //verifica se esta caindo e se o led não esta piscando
    estadoP3 = 1;
    digitalWrite(IGN_3, HIGH);
    timerP3 = millis() + intervaloParaquedas;
  }
  if (estadoP3 == 1 && (millis() >= timerP3)) {
    estadoP3 = 2;
    digitalWrite(IGN_3, LOW);
  }

  //acionando quarto paraquedas
  if (queda && estadoP4 == 0 && alturaFiltrada_1 < -3) {
    timerP4 = millis() + atrasoP1_P4;
    estadoP4 = 1;
  }
  if (estadoP4 == 1 && (millis() >= timerP4)) {
    estadoP4 = 2;
    digitalWrite(IGN_4, HIGH);
    timerP4 = millis() + intervaloParaquedas;
  }
  if (estadoP4 == 2 && (millis() >= timerP4)) {
    estadoP4 = 3;
    digitalWrite(IGN_4, LOW);
  }

  //salvando dados no sd
  String dataString = "";
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(altura) + "\t";
  dataString += String(alturaFiltrada_0) + "\t";
  dataString += String(alturaFiltrada_1) + "\t";
  dataString += String(fallenCondition) + "\t";
  dataString += String(contador) + "\t";
  dataString += String(estadoP1) + "\t";
  dataString += String(estadoP2) + "\t";
  dataString += String(estadoP3) + "\t";
  dataString += String(estadoP4) + "\t";
  Serial.println(dataString);
  arquivo = SD.open(nomeArquivo, FILE_WRITE);
  if (arquivo) {
    arquivo.println(dataString);
    arquivo.close();
  }
}