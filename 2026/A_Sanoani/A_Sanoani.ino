#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  Serial.println("Temperature(*C), Pressure(Pa), Altitude(meters), Pressure at sealevel (calculated)(Pa), Real altitude(meters)")
  }
}

  
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);  // change state of the LED by setting the pin to the HIGH voltage level
    delay(2000);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // change state of the LED by setting the pin to the LOW voltage level
    delay(2000);
    Serial.print(bmp.readTemperature());
    Serial.print(", ");
    
    Serial.print(bmp.readPressure());
    Serial.print(", ");
    
    Serial.print(bmp.readAltitude());
    Serial.print(", ");

    Serial.print(bmp.readSealevelPressure());
    Serial.print(", ");

    Serial.println(bmp.readAltitude(101500));
    
    delay(500);
}