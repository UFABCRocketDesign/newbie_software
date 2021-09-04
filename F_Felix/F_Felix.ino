#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
  
    Serial.begin(9600);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    }
}

// the loop function runs over and over again forever
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

    Serial.print("Temperature (*C) ");
    Serial.print(" Pressure (Pa) ");
    Serial.print("Altitude (meters)  ");
    Serial.print("Pressure at sealevel (Pa)  ");
    Serial.println("Real altitude (meters)  ");
    
    Serial.print(bmp.readTemperature());
    
    Serial.print(bmp.readPressure());
    
    Serial.print(bmp.readAltitude());
   
    Serial.print(bmp.readSealevelPressure());
    
    Serial.print(bmp.readAltitude(101500));
    
    
    Serial.println();
    delay(500);
}
