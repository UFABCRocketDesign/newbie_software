#include <SD.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

int chipSelect = 53;
float altitudeInicial;
float somaAltitude;
float listaSuavizarCurva_0[10];
float listaSuavizarCurva_1[5];
float listaDeteccaoQueda[2];
int contador = 0;
float alturaAnterior;
String numA;
File arquivo;
String nomeArquivo;
String inicio = "data";
String tipoDeArquivo = ".txt";

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
//abrindo o SD
  pinMode(chipSelect,OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("Falha ao inicializar o cartão SD");
    return;
  }
  Serial.println("Cartão SD inicializado com sucesso");

//verificando nome de arquivo
  int tamInicio = inicio.length();
  numA = "";
  for (int i = tamInicio; i < 8; i++){
    numA += "0"; //concatenando 0 a string
  }

  nomeArquivo = inicio + String(numA) + tipoDeArquivo;

  while (SD.exists(nomeArquivo)) {
    nomeArquivo = inicio + String(numA) + tipoDeArquivo;
    numA = String(numA.toInt() + 1);
  }
  Serial.print(nomeArquivo);
  arquivo = SD.open(nomeArquivo, FILE_WRITE);
  if (arquivo) {
    String header = "Temperature(C), Pressure(Pa), High(meters), Filtered High 0(meters), Filtered High 1(meters), Fallen(1)/ Not fallen (0), Contador de Queda";
    arquivo.println(header);
    arquivo.close();
  } else {
    Serial.println("Erro ao abrir o arquivo");
  }

//end 
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Não foi possível achar um sensor BMP085 válido, verifique os fios!");
    while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Filtered High 0(meters)\t Filtered High 1(meters)\t Fallen(1)/ Not fallen (0)\t Contador de Queda");

  somaAltitude = 0;
  for (int posicaoListaAltitude = 0; posicaoListaAltitude < 10; posicaoListaAltitude++) {
    somaAltitude += bmp.readAltitude();
  }

  altitudeInicial = somaAltitude / 10;
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

//salvando dados no sd
  String dataString = "";
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(altura) + "\t";
  dataString += String(alturaFiltrada_0) + "\t";
  dataString += String(alturaFiltrada_1) + "\t";
  dataString += String(fallenCondition) + "\t";
  dataString += String(contador) + "\t";
  Serial.println(dataString);
  arquivo = SD.open(nomeArquivo, FILE_WRITE);
  if(arquivo){
    arquivo.println(dataString);
    arquivo.close();
  }

}