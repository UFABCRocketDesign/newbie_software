#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float altitudeInicial; //definiçao da altura inicial
float altitudeAprox[10]; //definição da altura suave
float altitudeFiltrado[10]; //definição da altura suave 2




float altitudeMaxima = 0.0;
bool emQueda = false;
const float margemQueda = 1.5;




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


  for (int i = 0; i < 9; i++) {
    altitudeAprox[i] = altitudeAprox[i + 1];
  }
  altitudeAprox[9] = altura;
  float somaAprox = 0;
  for (int i = 0; i < 10; i++) {
    somaAprox += altitudeAprox[i];
  }
  float altitudeSuave = somaAprox / 10;
  Serial.print(altitudeSuave);
  Serial.print("\t");

for (int i = 0; i < 9; i++) {
    altitudeFiltrado[i] = altitudeFiltrado[i + 1];
  }
  altitudeFiltrado[9] = altitudeSuave;
  float somaFiltrado = 0;
  for (int i = 0; i < 10; i++) {
    somaFiltrado += altitudeFiltrado[i];
  }
  float altitudeFinal = somaFiltrado / 10;
  Serial.print(altitudeFinal);
  Serial.print("\t");




if (altitudeFinal > altitudeMaxima) {
    altitudeMaxima = altitudeFinal;
}
if (!emQueda && altitudeFinal < (altitudeMaxima - margemQueda)) {
    //if (altitudeMaxima > 5.0) {
      emQueda = true;
    }
  
  Serial.print(emQueda); 



  Serial.print("\t");
  Serial.println();
}