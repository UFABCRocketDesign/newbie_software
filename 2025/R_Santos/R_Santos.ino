#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  float solo = bmp.readAltitude();
  Serial.println("Temperature(C)|Pressure(PA)|Altitude(Pa)|Pressure at Sea Level(PA)|Real Altitude(m)|Altura Atual(m)");
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}
  
void loop() {
    float altitude = bmp.readAltitude() - solo;
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude());
    Serial.print("\t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    Serial.println(altitude);

}

