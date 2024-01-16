#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float AltInicial = 0;
int numLeiturasInicial = 20;
float somaAltInicial = 0;

float altitudes[5];
int frente = 0;
int numLeiturasFila = 5;
float rawAltitude = 0;

void setup() {
  //BME085
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  //Cabeçalho
  Serial.println("Temperature (*C) \t Pressure (Pa) \t Altitude (m) \t Pressure at sea level (Pa) \t Raw Altitude \t Real Altitude (m)");

  //Leituras iniciais
  for (int i = 0; i < numLeiturasInicial; i++) {
    somaAltInicial += bmp.readAltitude();
  }

  AltInicial = somaAltInicial / numLeiturasInicial; //Médias das leituras iniciais

  //Inicializar o array com as primeiras 10 após a AltInicial
  for (int i = 0; i < numLeiturasFila; i++) {
    altitudes[i] = bmp.readAltitude();
  }
}

// the loop function runs over and over again forever
void loop() {

  //BME085
  Serial.print(bmp.readTemperature());
  Serial.print('\t');
  Serial.print(bmp.readPressure());
  Serial.print('\t');
  Serial.print(bmp.readAltitude());
  Serial.print('\t');
  Serial.print(bmp.readSealevelPressure());
  Serial.print('\t');
  
  rawAltitude = bmp.readAltitude() - AltInicial;
  Serial.print(rawAltitude); //Altura do sensor sem filtro, rawww
  Serial.print('\t');

  //Suavizar as leituras da altura utilizando o conceito de "fila"

  altitudes[frente] = rawAltitude;  // Adicionar a nova leitura na posição da frente do array

  frente = (frente + 1) % numLeiturasFila;  // Atualizar a posição da frente

  float somaAltRelativa = 0;
  for (int i = 0; i < numLeiturasFila; i++) { //
    somaAltRelativa += altitudes[i];
  }

  Serial.println((somaAltRelativa / numLeiturasFila));  //Altura do sensor real, com filtro
}