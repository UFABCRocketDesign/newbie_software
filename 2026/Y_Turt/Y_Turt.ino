#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float altitudeInicial;
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  altitudeInicial = bmp.readAltitude();
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
  float altitudeRelativa = bmp.readAltitude() - altitudeInicial;
  Serial.print(altitudeRelativa);
  Serial.print("\t");
  Serial.println();
}