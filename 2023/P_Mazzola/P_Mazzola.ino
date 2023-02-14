#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.println("temperatura"); Serial.println( "pressao"); Serial.println("altitude"); Serial.println("pressao no nvl do mar"); Serial.println ("real altitude");
}
  
void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("  ");

    Serial.print(bmp.readPressure());
    Serial.print("  ");
    

    Serial.print(bmp.readAltitude());
    Serial.print("  ");


    Serial.print(bmp.readSealevelPressure());
    Serial.print("  ");



    Serial.print(bmp.readAltitude(101500));
    Serial.print("  ");

    
    Serial.println();
    delay(5);
}