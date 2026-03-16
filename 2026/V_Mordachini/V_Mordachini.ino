// Victor :)
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float altitudeInicial;
float altitudeAtual;
float altura;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  altitudeInicial = bmp.readAltitude(); //altura 0

  Serial.println("Temp(C)\tPress(Pa)\tAlt(m)");
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
