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

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(2500);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1700);                      // wait for a second

    Serial.print("Temperature = ");
    Serial.print("Pressure = ");
    Serial.print("Altitude = ");
    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print("Real altitude = ");

    Serial.print(bmp.readTemperature());
    Serial.print("  ");
    Serial.print(bmp.readPressure());
    Serial.print("  ");
    Serial.print(bmp.readAltitude());
    Serial.print("  ");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("  ");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("  ");
    

    Serial.println(" *C");
    Serial.println(" Pa");
    Serial.println(" meters");
    Serial.println(" Pa");
    Serial.println(" meters");

    
    
    
    Serial.println();
    delay(500);
}