#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float AltInicial = 0;
int numLeiturasInicial = 25;
float somaAltInicial = 0;

float fatorSuavizacao = 0.7; // fator de suavização, varia de 0 a 1, sendo que 1 da mais peso nas medidas recentes e 0 nas antigas
float MME = 0; // Média Móvel Exponencial
float apogeu = 820;

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

  //Suavizar as leituras da altura usando MME
  MME = fatorSuavizacao*rawAltitude + (1-fatorSuavizacao)*MME;
  Serial.println(MME);  //Altura do sensor real, com filtro

  if (MME >= apogeu) {
    Serial.println("APOGEU! ative alguma coisa com isso pfvr");
  } 
}