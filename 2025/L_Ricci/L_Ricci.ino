#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float alt = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.println("T\tP\tA\tPMar\tAReal\tAReal2");
  for (int i = 0; i < 150; i++) {
    alt += bmp.readAltitude();
  }
}

void loop() {

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude()-alt);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.println();
  
}
