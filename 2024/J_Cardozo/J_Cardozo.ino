#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.print("Temperature(*C) |");
  Serial.print("Pressure(Pa) |");
  Serial.print("Altitude(m) |");
  Serial.print("Pressure at sealevel (calculated)(Pa) |");
  Serial.print("Real altitude(m)|");

  Serial.println("");
}

void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print(" |");
  Serial.print(bmp.readPressure());
  Serial.print(" |");
  Serial.print(bmp.readAltitude());
  Serial.print(" |");
  Serial.print(bmp.readSealevelPressure());
  Serial.print(" |");
  Serial.print(bmp.readAltitude(101500));
  Serial.print(" |");

  Serial.println();
  delay(500);      
}
