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
String header = "Temperature(C)\tPressure(Pa)\tHigh(meters)\tFiltered High 0(meters)\tFiltered High 1(meters)\tFallen(1)/ Not fallen (0)\tContador de Queda\tBlinking";
float inicioBlink;
int intervaloBlink = 1000;
bool pinoBlinking = 0;  // estado de piscar


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

  // determinando altitude inicial
  somaAltitude = 0;
  for (int posicaoListaAltitude = 0; posicaoListaAltitude < 10; posicaoListaAltitude++) {
    somaAltitude += bmp.readAltitude();
  }
  altitudeInicial = somaAltitude / 10;

  //definindo pino como porta de saída
  pinMode(IGN_1, OUTPUT);
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
  for (int i = 0; i < 1; i++){
    if (fallenCondition == 1 && pinoBlinking) {  //verifica se esta caindo e se o led não esta piscando
    pinoBlinking = 1;
    digitalWrite(IGN_1, pinoBlinking);
    inicioBlink = millis();
    if (pinoBlinking && (millis() - inicioBlink >= intervaloBlink)) {
      pinoBlinking = 0;
      digitalWrite(IGN_1, pinoBlinking);
    }
  }
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
  dataString += String(pinoBlinking) + "\t";
  Serial.println(dataString);
  arquivo = SD.open(nomeArquivo, FILE_WRITE);
  if (arquivo) {
    arquivo.println(dataString);
    arquivo.close();
  }
}