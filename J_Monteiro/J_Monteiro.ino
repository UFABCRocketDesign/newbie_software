#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  
  }
  
  Serial.print("Temperature =  \t");
  Serial.print(" Pa \t");
  Serial.print("  Pressure = \t");
  Serial.print(" Pa \t");
  Serial.print("  Pressure at sealevel (calculated) = \t");
  Serial.print(" Pa \t");
  Serial.print("  Real altitude = \t");
  Serial.println(" meters \t");
}
  
void loop() {
    
    Serial.print(bmp.readTemperature() );
    Serial.print('\t');
    
    Serial.print( bmp.readPressure()  );
    Serial.print('\t');
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal 
    Serial.print( bmp.readAltitude());
    Serial.print('\t');
   
    Serial.print( bmp.readSealevelPressure() );
    Serial.print('\t');
   

  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  
    Serial.print( bmp.readAltitude(101500));
    Serial.print('\t');
    
    Serial.println();
 
}
