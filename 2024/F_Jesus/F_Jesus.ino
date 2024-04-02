#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  Serial.print("Temperature");
  Serial.print(" *C");
  Serial.print("Pressure");
  Serial.print(" Pa");
  Serial.print("Altitude");
  Serial.print(" meters");
  Serial.print("Pressure at sealevel (calculated)");
  Serial.print(" Pa");
  Serial.print("Real altitude = ");
  Serial.println(" meters");
}
void loop() {
    
  Serial.print(bmp.readTemperature( ));
  Serial.print(bmp.readPressure( ));
  Serial.print(bmp.readAltitude( ));
  Serial.print(bmp.readSealevelPressure( ));
  Serial.println(bmp.readAltitude(101500));
}