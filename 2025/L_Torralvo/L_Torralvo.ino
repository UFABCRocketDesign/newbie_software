#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}

void loop() {

    /*cabeçalho!*/
    Serial.println("Temperature\tPressure\tAltitude\tPressure at sealevel\tReal altitude");

    /*Valores*/
    Serial.print(bmp.readTemperature()); Serial.print("\t");
    
    Serial.print(bmp.readPressure()); Serial.print("\t");

    Serial.print(bmp.readAltitude()); Serial.print("\t");

    Serial.print(bmp.readSealevelPressure()); Serial.print("\t");

    Serial.println(bmp.readAltitude(101500)); Serial.print("\t");
    
    Serial.println();
    delay(500);                
}