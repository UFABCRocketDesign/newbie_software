#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  if (!bmp.begin()) {
	  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	  while (1) {}
  }
}
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  Serial.print("Temperature (*C); Pressure (Pa); Altitude (m); Pressure At Sea Level (Pa); Real Altitude (m);");
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
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}