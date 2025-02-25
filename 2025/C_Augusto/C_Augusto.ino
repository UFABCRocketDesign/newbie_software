#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
float altitudeTarada;


void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
	while (1) {}
  }  
  altitudeTarada = bmp.readAltitude(101500);  
  
  

Serial.println("Temperatura \t Pressao \t Altitude \t Nivel do mar \t Altitude Real");
}
  

void loop() {
    Serial.print( bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500) - altitudeTarada);
    Serial.print("\t");
    Serial.println();
}
