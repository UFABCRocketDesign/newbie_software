#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float AltInicial = 0;
int numLeiturasInicial = 25;
float somaAltInicial = 0;

// Variáveis para a média simples
const int numLeituras = 25; // número de leituras para a média
float leituras[numLeituras]; // as leituras do sensor de altitude
int indiceLeitura = 0; // o índice da leitura atual
float somaLeituras = 0; // a soma das leituras
float mediaAltitude = 0; // a média das leituras

//apogeu
float apogeu = 10;
int contador = 0;

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

  //Armazena 25 leituras para realizar a média
  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - AltInicial;
    somaLeituras += leituras[i];
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
  
  float rawAltitude = bmp.readAltitude() - AltInicial;
  Serial.print(rawAltitude); //Altura do sensor sem filtro, rawww
  Serial.print('\t');

  // Filtro
  somaLeituras = somaLeituras - leituras[indiceLeitura];
  leituras[indiceLeitura] = rawAltitude;
  somaLeituras = somaLeituras + leituras[indiceLeitura];
  indiceLeitura++;
 
  if (indiceLeitura >= numLeituras) { //se for o último vetor, volta para o início
    indiceLeitura = 0;
  }

  //Média
  mediaAltitude = somaLeituras / numLeituras;
  Serial.println(mediaAltitude);

  if (mediaAltitude >= apogeu) {
    contador++;
    if (contador > 20) {
      Serial.println("APOGEU! ative alguma coisa com isso pfvr");
    }
  } 
}