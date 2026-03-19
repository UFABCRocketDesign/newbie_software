#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   PressaoMar    |")
}
  
void loop() {
    
    Serial.print(bmp.readTemperature(), bmp.readPressure(), bmp.readAltitude, bmp.readSealevelPressure(), bmp.readAltitude(101500));
    
}