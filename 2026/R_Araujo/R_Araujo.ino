#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float t0;
float t1;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
    Serial.print("Temperature\t");
    Serial.print("Pressure\t");
    Serial.print("Altitude\t");
    Serial.print("Pressure at sealevel (calculated)\t ");
    Serial.println("Real altitude");
  t0=bmp.readAltitude();
}
void loop() {
  t1=bmp.readAltitude();
    Serial.print(bmp.readTemperature());
    Serial.print('\t');
    Serial.print(bmp.readPressure());
    Serial.print('\t');
    Serial.print(t1-t0);
    Serial.print('\t');
    Serial.print(bmp.readSealevelPressure());
    Serial.print('\t');
    Serial.println(bmp.readAltitude(101500)); 
}