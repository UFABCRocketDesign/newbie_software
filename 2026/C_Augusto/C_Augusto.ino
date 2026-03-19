#include <Adafruit_BMP085.h>
float altura0;
float alturasoma;

Adafruit_BMP085 bmp;
  
void setup() {
  int contador = 0;
  float alturasoma = 0;
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   ");
  for(int i = 0; i == 9; i++){
    alturasoma += bmp.readAltitude();
  }
}
  
void loop() {
    float alturatarada = bmp.readAltitude() - alturasoma/9;

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.println();
    
    alturasoma += bmp.readAltitude();
}