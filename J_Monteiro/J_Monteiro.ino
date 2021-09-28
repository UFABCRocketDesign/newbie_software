#include <Adafruit_BMP085.h>
  
Adafruit_BMP085 bmp;



void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
 Serial.println("Could not find a valid BMP085 sensor");
  while (1) {}
   pinMode(LED_BUILTIN, OUTPUT);
  }

  
 
}
  
void loop() {
  
  float relative_average;
  float altitude = bmp.readAltitude();
  float current_Altitude = bmp.readAltitude();
  float initialread = 0;
  float previous_Altitude;
  float altitude_max = 0;
 
  
    Serial.print("Temperature = ");
    Serial.print( bmp.readTemperature());
    Serial.print(" *C");
    
    Serial.print(  "  Pressure = ");
    Serial.print( bmp.readPressure());
    Serial.print(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(  "  Altitude = ");
    Serial.print( bmp.readAltitude());
    Serial.print(" meters");

    Serial.print(  " Pressure at sealevel (calculated) = ");
    Serial.print( bmp.readSealevelPressure());
    Serial.print( " Pa");
    

   
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("  Real altitude = ");
    Serial.print( bmp.readAltitude(101500));
    Serial.println(" meters");

    Serial.println();
    delay(500);
    
     
    

      for ( float i =0; i < 15; i ++){

             altitude_max = 0;
             ( current_Altitude >  altitude_max );

              altitude_max = current_Altitude;
              altitude_max = previous_Altitude;

               relative_average = (  previous_Altitude + current_Altitude )/ 2;     
        
        
        if (relative_average  < current_Altitude){
    
      Serial.print("Subida");
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000); 
    }
     else{

      Serial.print("Descida");
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000); 
      
        }
      }
      
     
}
     
