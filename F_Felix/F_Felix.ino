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

    Serial.print("Temp.(*C) ");
    Serial.print(" Pres.(Pa) ");
    Serial.print("Alt.(m)  ");
    Serial.print("Pres. sealevel(Pa)  ");
    Serial.println("Real alt.(m)  ");
}

// the loop function runs over and over again forever
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    
    Serial.print(bmp.readTemperature());
    Serial.print("         ");
    
    Serial.print(bmp.readPressure());
    Serial.print("         ");
    
    Serial.print(bmp.readAltitude());
    Serial.print("         ");
   
    Serial.print(bmp.readSealevelPressure());
    Serial.print("            ");
    
    Serial.print(bmp.readAltitude(101500));
    Serial.println("        ");
   
    delay(500);
}
