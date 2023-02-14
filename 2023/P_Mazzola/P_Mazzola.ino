#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.println('temperatura'); Serial.println( 'pressao'); Serial.println('altitude'); Serial.println('pressao no nvl do mar'); Serial.println ('real altitude');
}
  
void loop() {
    Serial.print(bmp.readTemperature());
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  
    delay(1000); 
    

    Serial.print(bmp.readPressure());
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  
    delay(1000); 
    

    Serial.print(bmp.readAltitude());
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  
    delay(1000); 


    Serial.print(bmp.readSealevelPressure());
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  
    delay(1000); 



    Serial.print(bmp.readAltitude(101500));
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  
    delay(1000); 
    
    Serial.println();
    delay(500);
}