#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
float pressaoTara;


void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
	while (1) {}
  }
pressaoTara = bmp.readPressure();  

Serial.println("Temperatura \t Pressao \t Altitude \t Nivel do mar \t Altitude Real");
}
  

void loop() {
    Serial.print( bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(pressaoTara));
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    Serial.println();
}
