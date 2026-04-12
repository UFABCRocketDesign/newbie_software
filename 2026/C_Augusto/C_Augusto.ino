//#include <Adafruit_BMP085.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
float altura0;
float alturasoma = 0;
float alturatarada;
float alpha = 0.2;   // fator de suavização
float filtrado = 0;  // valor inicial
float filtrado2 = 0; //valor inicial2
float filtrado3 = 0;
float apogeu = 0;
float aux = 0;


//Adafruit_BMP085 bmp;
Adafruit_BMP280 bmp;
  
void setup() {
  int contador = 0;
  Serial.begin(115200);
  delay(1000);
  Wire.begin(21,22);
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

  filtrado = alpha * alturatarada + (1 - alpha) * aux;
  aux = filtrado;
  
  filtrado2 = alpha * filtrado + (1-alpha) * filtrado2;
  filtrado3 = alpha * filtrado2 + (1-alpha) * filtrado3;
  
if (filtrado3 > apogeu) {
  apogeu = filtrado3;
}

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.print("\t");
    Serial.print(filtrado3);
    Serial.print("\t");
    Serial.print(apogeu);
    Serial.println();
    delay(20);
}