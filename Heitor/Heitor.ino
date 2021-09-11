#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float solo = 0.0;

void setup() {
  Serial.begin(250000);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
//   Serial.print("*C\tPa\tm\t" );
  Serial.print("m");
  for(int i=0; i<100; i++)
  {
    solo += bmp.readAltitude();
  }
  solo /= 100.0;
}

void loop() {

  float temp = bmp.readTemperature();
  int press = bmp.readPressure();
  float alt = bmp.readAltitude();
//   Serial.print(temp);
//   Serial.print("\t");
//   Serial.print(press);
//   Serial.print("\t");
  Serial.print(alt-solo);
  Serial.print("\t");

  Serial.println();
}
