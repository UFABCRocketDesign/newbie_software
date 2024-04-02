#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float AltIni = 0;
int nLeitIni = 15;
float somaAltIni = 0;
float altitudes[10];
int frente = 0;
int nLeitFila = 10;
float rawAltitude = 0;

//variáveis para a média simples:
const int numLeituras = 25; //leituras para a média
float leituras[numLeituras]; //leituras do sensor de altitude
int indiceLeitura = 0; //leitura atual
float somaLeituras = 0; //soma das leituras
float mediaAltitude = 0; //média das leituras


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  if (!bmp.begin()) {

    Serial.println("Could not find a valid BMP085 sensor, check wiring!");

    while (1) {}
  }
  Serial.println("Temperature (*C) \t Pressure (Pa) \t Altitude (m) \t Pressure at sea level (Pa) \t Raw Altitude \t Real Altitude (m)"); //cabeçalho

  for (int i = 0; i < nLeitIni; i++) {
    somaAltIni += bmp.readAltitude(); //leituras inciais
  }

  AltIni = somaAltIni / nLeitIni; //média leituras

  for (int i = 0; i < numLeituras; i++) {
  altitudes[i] = bmp.readAltitude() - AltIni;
  somaLeituras += altitudes[i];
 }

}
void loop() {

  Serial.print(bmp.readTemperature()); //lendo temperatura
  Serial.print("\t");
  Serial.print(bmp.readPressure()); //lendo pressão
  Serial.print("\t");
  Serial.print(bmp.readAltitude()); //lendo altitude
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure()); //lendo altitude no nível do mar
  Serial.print("\t");

  float rawAltitude = bmp.readAltitude() - AltIni; //calculando a altura
  Serial.print(rawAltitude); //altura sem filtro
  Serial.print('\t');

  //filtro:
  somaLeituras = somaLeituras - altitudes[indiceLeitura];
  altitudes[indiceLeitura] = rawAltitude;
  somaLeituras = somaLeituras + altitudes[indiceLeitura];
  indiceLeitura++;
  if (indiceLeitura >= numLeituras) { //quando chegar no último vetor - loop
    indiceLeitura = 0;
  }

  //Média
  mediaAltitude = somaLeituras / numLeituras;
  Serial.println(mediaAltitude);

  Serial.println();


}