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
  delay(2500);
  Serial.print("Temperature = ");
  Serial.println(bmp.readTemperature());
  Serial.print("Pressure = ");
  Serial.println(bmp.readPressure());
  Serial.print("Altitude = ");
  Serial.println(bmp.readAltitude());
  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.println(bmp.readSealevelPressure());
  Serial.print("Real altitude = ");
  Serial.print(bmp.readAltitude(101500));
  Serial.println();
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(5000);
}