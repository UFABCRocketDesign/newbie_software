
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(10);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(2000); 
 
    Serial.print(bmp.readTemperature());
   
    Serial.print(bmp.readPressure());
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
 
    Serial.print(bmp.readAltitude());
   
    Serial.print(bmp.readSealevelPressure());
   
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.

    Serial.print(bmp.readAltitude(101500));
    
    Serial.println();
    delay(500);
                     // wait for a second
}
