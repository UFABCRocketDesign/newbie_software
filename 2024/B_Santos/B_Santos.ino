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
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(2000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(8000);                      // wait for a second

  Serial.print(bmp.readTemperature());
    Serial.print(" *C");
    
    Serial.print(bmp.readPressure());
    Serial.print(" Pa");
    
    Serial.print(bmp.readAltitude());
    Serial.print(" meters");

    Serial.print(bmp.readSealevelPressure());
    Serial.print(" Pa");

    Serial.print(bmp.readAltitude(101500));
    Serial.print(" meters");
    
    Serial.println();
    delay(500);
}