#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
      Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.print(" *C/ ");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.print(" Pa/ ");
    
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.print(" meters/ ");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.print(" Pa/ ");

    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters "); 
}
void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print('/');
    Serial.print(bmp.readPressure());
    Serial.print('/');
    Serial.print(bmp.readAltitude());
    Serial.print('/');
    Serial.print(bmp.readSealevelPressure());
    Serial.print('/');
    Serial.println(bmp.readAltitude(101500)); 
}