#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Pressure at sealevel (calculated, Pa)\t Real altitude(meters)");
}
  float AltitudeInicial= bmp.readAltitude();


void loop() {

    float Altura= bmp.readAltitude()- AltitudeInicial;

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(Altura);
    Serial.print("\t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    
    Serial.println();
}