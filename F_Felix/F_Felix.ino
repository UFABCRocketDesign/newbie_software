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

    Serial.print("Temp.(*C)\t");
    Serial.print("Pres.(Pa)\t");
    Serial.print("Alt.(m)\t");
    Serial.print("Pres. sealevel(Pa)\t");
    Serial.println("Real alt.(m)\t");
}

// the loop function runs over and over again forever
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    
    Serial.print(bmp.readAltitude());
    Serial.print("\t");
   
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    
    Serial.print(bmp.readAltitude(101500));
    Serial.println("\t");
   
    delay(500);
}
