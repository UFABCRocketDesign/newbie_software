#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(250000);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.print("*C\tPa\tm\t");
}

void loop() {

  float temp = bmp.readTemperature();
  int press = bmp.readPressure();
  float alt = bmp.readAltitude();
  Serial.print(temp);
  Serial.print("\t");
  Serial.print(press);
  Serial.print("\t");
  Serial.print(alt);
  Serial.print("\t");

  Serial.println();
}
