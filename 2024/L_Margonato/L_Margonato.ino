#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float local;
float reallocal;

void setup() {

  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  local = bmp.readAltitude();
  reallocal = bmp.readAltitude(101500);

  Serial.print("Temperature:");
  Serial.print("\t");
  Serial.print("Pressure:");
  Serial.print("\t");
  Serial.print("Altitude:");
  Serial.print("\t");
  Serial.print("Pressure at sealevel:");
  Serial.print("\t");
  Serial.println("Real altitude:");

}
  
void loop() {
                 
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude()-local);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.println(bmp.readAltitude(101500)-reallocal);
    
  delay(500);
}