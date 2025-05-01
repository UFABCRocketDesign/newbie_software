#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}
  
void loop() {
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    digitalWrite(LED_BUILTIN, HIGH);  
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(1000);

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(1000);

    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(1000);

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(1000);

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(1000);
    Serial.println();
    delay(500);
}
