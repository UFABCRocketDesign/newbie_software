#include <Adafruit_BMP085.h>
float altura0;
int contador;

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   ");
  while (contador <= 9){
    altura0 = bmp.readAltitude();
  }
}
  
void loop() {
    
    float alturatarada = bmp.readAltitude() - altura0;

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.println();
    contador += 1;
}