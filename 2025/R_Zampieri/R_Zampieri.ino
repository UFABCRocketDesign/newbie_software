#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  Serial.print("Temperatura/t");
  Serial.print("Pressão/t");
  Serial.print("Altitude/t");
  Serial.print("Pressão no Nível do Mar/t");
  Serial.print("Altitude Real/t");
}


void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print("/t");
  Serial.print(bmp.readPressure());
  Serial.print("/t");
  Serial.print(bmp.readAltitude());
  Serial.print("/t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("/t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("/t");

  Serial.println();
}
