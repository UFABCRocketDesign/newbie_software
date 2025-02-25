#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.println("T\tP\tA\tPMar\tAReal\tAReal2");
}

void loop() {

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");

  // Possibilidades: Pegar o valor de pressão e substituir em readAltitude e ver o que acontece 
  // Serial.print(bmp.readAltitude(101500));
  float pressure = bmp.readPressure();
  Serial.print(bmp.readAltitude(pressure));
  Serial.print(bmp.readAltitude(92765.2));
  
  Serial.println();
  
}