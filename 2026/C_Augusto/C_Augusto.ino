//#include <Adafruit_BMP085.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
float altura0;
float alturasoma = 0;
float alturatarada;
float alpha = 0.2;   // fator de suavização
float filtrado = 0;  // valor inicial
float filtrado2 = 0; //valor inicial2
float apogeu = 0;


//Adafruit_BMP085 bmp;
Adafruit_BMP280 bmp;
  
void setup() {
  int contador = 0;
  Serial.begin(115200);
  Wire.begin(42,41);
  if (!bmp.begin(0x76)) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   Filtro    |   Apogeu    ");
  for(int i = 0; i < 9; i++){
    alturasoma += bmp.readAltitude();
  }
  alturasoma = alturasoma/9; 
}
  
void loop() {
  float alturatarada = bmp.readAltitude() - alturasoma;

  filtrado = alpha * alturatarada + (1 - alpha) * filtrado;
  filtrado2 = alpha * filtrado + (1-alpha) * filtrado2;
  
if (filtrado2 > apogeu) {
  apogeu = filtrado2;
}

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.print("\t");
    Serial.print(filtrado2);
    Serial.print("\t");
    Serial.print(apogeu);
    Serial.println();
}