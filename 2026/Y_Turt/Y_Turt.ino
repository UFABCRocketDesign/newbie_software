#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float altitudeInicial; //definiçao da altura inicial
float altitudeAprox[20]; //definição da altura suave

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
    float altura = bmp.readAltitude() - altitudeInicial; 
    Serial.print(altura);
    Serial.print("\t"); 
  for (int i = 0; i < 19; i++) {
    altitudeAprox[i] = altitudeAprox[i + 1];
  }
  altitudeAprox[19] = altura;
  float somaAprox = 0;
  for (int i = 0; i < 20; i++) {
    somaAprox += altitudeAprox[i];
  }
  float altitudeSuave = somaAprox / 20;
  Serial.print(altitudeSuave);
  Serial.print("\t");
  Serial.println();
}