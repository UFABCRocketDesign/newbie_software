#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
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
  Serial.print(" *C / Pa / meters / Pa / meters ");
    Serial.print(bmp.readTemperature());
    
    Serial.print(bmp.readPressure());
    
    Serial.print(bmp.readAltitude());

    Serial.print(bmp.readSealevelPressure());

    Serial.print(bmp.readAltitude(101500));
    
    Serial.println();
}