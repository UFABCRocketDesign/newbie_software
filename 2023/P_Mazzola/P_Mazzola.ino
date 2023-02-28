#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.print("temperatura "); Serial.print( "pressao  "); Serial.print("altitude  "); Serial.print("pressao no nvl do mar "); Serial.print ("real altitude  ");
}
  
void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("  ");

    Serial.print(bmp.readPressure());
    Serial.print("  ");
    

    Serial.print(bmp.readAltitude()-bmp.readAltitude());
    Serial.print("  ");
    
    Serial.println();
    delay(5);
}