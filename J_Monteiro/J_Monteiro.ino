#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
  while (1) {}
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  Serial.print("Temperature = ");
  Serial.print(" Pa");
  Serial.print("  Pressure = ");
  Serial.print(" Pa");
  Serial.print("  Pressure at sealevel (calculated) = ");
  Serial.print(" Pa");
  Serial.print("  Real altitude = ");
  Serial.println(" meters");
  }
}
  
void loop() {
    
    Serial.print(bmp.readTemperature());
    
    Serial.print( bmp.readPressure());
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal 
    Serial.print( bmp.readAltitude());
   
    Serial.print( bmp.readSealevelPressure());
   

  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  
    Serial.println( bmp.readAltitude(101500));
    
    Serial.println();
    delay(100);
}
