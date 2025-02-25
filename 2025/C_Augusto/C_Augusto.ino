#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float altitudeTarada = 0;  

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
    while (1) {}
  }  

  for (int i = 0; i < 10; i++) { 
    altitudeTarada += bmp.readAltitude(101500);

  }
  altitudeTarada /= 10; 

  Serial.println("Temperatura \t Pressao \t Altitude \t Nivel do mar \t Altitude Relativa");
}

void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");

    Serial.print(bmp.readPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude(101500) - altitudeTarada);
    Serial.println();

}
