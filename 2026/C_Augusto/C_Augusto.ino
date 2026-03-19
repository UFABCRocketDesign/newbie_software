#include <Adafruit_BMP085.h>
float altura0;
int contador;
float alturasoma;

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   ");
  if (contador == 9){
    alturamedia == alturasoma/9;
  }
}
  
void loop() {
    
    float alturatarada = bmp.readAltitude() - alturasoma;

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.println();
    
    contador += 1;
    alturasoma += bmp.readAltitude();
}