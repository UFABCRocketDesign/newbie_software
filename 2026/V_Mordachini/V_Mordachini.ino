// Victor :)
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float altitudeInicial;
float altitudeAtual;
float altura;

float somaCalibragem = 0;
const int medicoes = 30; // numero de leituras para calibragem

void setup() {

  Serial.begin(115200);

  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  //LOGICA DE CALIBRACAO:
  for (int i = 0; i < medicoes; i++) {
    somaCalibragem += bmp.readAltitude();
  }
  altitudeInicial = somaCalibragem / medicoes; //Altura 0

  //CABECALHO
  Serial.println("Temp(C)\tPressao(Pa)\tAltura(m)");
}

void loop() {
  altitudeAtual = bmp.readAltitude();
  altura = altitudeAtual - altitudeInicial;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(altura);
  Serial.println("\t");
  
         
}
