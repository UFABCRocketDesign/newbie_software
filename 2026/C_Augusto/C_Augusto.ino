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

    Serial.print(bmp.readPressure());
    Serial.print(bmp.readAltitude());
    Serial.print(bmp.readSealevelPressure());
    Serial.println(bmp.readAltitude(101500));
}