#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   PressaoMar    |");
}
  
void loop() {
    
    Serial.print(bmp.readTemperature(), "\t");
    Serial.print(bmp.readPressure(), "\t");
    Serial.print(bmp.readAltitude()), "\t";
    Serial.print(bmp.readSealevelPressure(), "\t");
    Serial.println(bmp.readAltitude(101500), "\t");
}