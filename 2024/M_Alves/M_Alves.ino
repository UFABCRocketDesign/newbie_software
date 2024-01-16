#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float AltInicial = 0;
int numLeituras = 100;
float somaAltInicial = 0;

void setup() {
  //BME085
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  //Cabeçalho
  Serial.println("Temperature (*C) \t Pressure (Pa) \t Altitude (m) \t Pressure at sea level (Pa) \t Real Altitude (m)");

  //Somar diversas "leituras iniciais" e tirar a média
  for(int i = 0; i < numLeituras; i++) {
    somaAltInicial += bmp.readAltitude();
    delay(10); // Aguarde um pouco entre as leituras
  }

  //Média das alturas
  AltInicial = somaAltInicial / numLeituras;

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
  Serial.println(bmp.readAltitude() - AltInicial); //Altura do sensor

}