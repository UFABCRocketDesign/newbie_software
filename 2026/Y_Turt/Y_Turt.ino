#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float altitudeInicial; //definiçao da altura inicial

//inicio de vetor e variavel acumulativa da altura relativa
float altitudeAprox[5];

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
float soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma/10; //definição da altura inicial
  
  Serial.println("*C\tPa\tmeters\tPa\tmeters");
}

void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
    //definição da altura relativa = 0
    float altitudeRelativa = bmp.readAltitude() - altitudeInicial; 
    Serial.print(altitudeRelativa); 
  for (int i = 0; i < 4; i++) {
    altitudeAprox[i] = altitudeAprox[i + 1];
    altitudeAprox[4] = altitudeRelativa;
  }
  float somaAprox = 0;
  for (int i = 0; i < 5; i++) {
    somaAprox += altitudeAprox[i];
  }
  float altitudeSuave = somaAprox / 5;
  Serial.print(altitudeSuave);
  Serial.print("\t");
  Serial.println();
}