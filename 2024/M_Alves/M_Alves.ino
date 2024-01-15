#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  //BME085
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  //Cabeçalho
  Serial.println("Temperature (*C); Pressure (Pa); Altitude (m); Pressure at sea level (Pa); Real Altitude (m);");
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
  Serial.print(bmp.readAltitude(101500));
  Serial.println('\t');

  delay(50);
}