#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  else {
  Serial.print("Temperature(C)|");
  Serial.print("Pressure(Pa)|");
  Serial.print("Altitude(meters)|");
  Serial.print("Pressure at sealevel (calculated, Pa)|");
  Serial.print("Real altitude(meters)");
  }
}


void loop() {

    Serial.print(bmp.readTemperature(),"   |");
    Serial.print(bmp.readPressure(),"   |");
    Serial.print(bmp.readAltitude(),"   |");
    Serial.print(bmp.readSealevelPressure(),"   |   ");
    Serial.print(bmp.readAltitude(101500),"/n");
  

    Serial.println();
}