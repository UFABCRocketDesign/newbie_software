#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float altitudeInicial;
float somaAltitude;
float listaSuavizarCurva_0[10];
float listaSuavizarCurva_1[5];
float listaDeteccaoQueda[2];
int contador = 0;


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
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Fallen(1)/ Not fallen (0)\t Contador de Queda");

  somaAltitude = 0;
  for (int posicaoListaAltitude = 0; posicaoListaAltitude < 10; posicaoListaAltitude++) {
    somaAltitude += bmp.readAltitude();
  }

  altitudeInicial = somaAltitude / 10;
}

void loop() {

  float altura = bmp.readAltitude() - altitudeInicial;
  float alturaFiltrada_0 = filtroSuavizarCurva_0(altura);
  float alturaFiltrada_1 = filtroSuavizarCurva_1(alturaFiltrada_0);
  float alturaAnterior;
  int fallenCondition = 0;


  for (int i = 1; i < 2; i++) {
    listaDeteccaoQueda[0] = alturaFiltrada_1;
    listaDeteccaoQueda[1] = alturaAnterior;
  }
  if (alturaFiltrada_1 < alturaAnterior) {
    contador++;
  } else {
    contador = 0;
  }
  if (contador >= 5) {
    fallenCondition = 1;
  } else {
    fallenCondition = 0;
  }

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(altura);
  Serial.print("\t");
  Serial.print(alturaFiltrada_0);
  Serial.print("\t");
  Serial.print(alturaFiltrada_1);
  Serial.print("\t");
  Serial.print(fallenCondition);
  Serial.print("\t");
  Serial.print(contador);

  Serial.println();
}