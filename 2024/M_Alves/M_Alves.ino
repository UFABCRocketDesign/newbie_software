#include <Adafruit_BMP085.h>
#define LED 13

Adafruit_BMP085 bmp;

void setup() {
  //LED
  pinMode(LED, OUTPUT);

  //BME085
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.print("Temperature (*C); Pressure (Pa); Altitude (m); Pressure at sea level (Pa); Real Altitude (m);");
}

// the loop function runs over and over again forever
void loop() {

  digitalWrite(LED, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(50);                      // wait for a second

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//

  //BME085
  Serial.print(bmp.readTemperature());
  Serial.print("; ");
  Serial.print(bmp.readPressure());
  Serial.print("; ");
  Serial.print(bmp.readAltitude());
  Serial.print("; ");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("; ");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("; ");
  Serial.println();

  digitalWrite(LED, LOW);   // turn the LED off by making the voltage LOW
  delay(3000);
}