#include <Adafruit_BMP085.h>
float altura0;
float alturasoma = 0;
float alturatarada;
float RS = 0.5; //ruido fisico
float RM = 0.9; //ruido da medicao
float c = 1; //confiança do filtro
float g = 0.3;//ganho do filtro
float filtrocalman = 0;

Adafruit_BMP085 bmp;
  
void setup() {
  int contador = 0;
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   Filtro    ");
  for(int i = 0; i < 9; i++){
    alturasoma += bmp.readAltitude();
  }
  alturasoma = alturasoma/9; 
}
  
void loop() {
  float alturatarada = bmp.readAltitude() - alturasoma;

  //algoritmo do meu mano calman
  c = c + RS;
  g = c/(c+RM);
  filtrocalman = alturatarada - g*(alturatarada);
  c = (1-g)*c;


    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.print("\t");
    Serial.print(filtrocalman);
    Serial.println();
}