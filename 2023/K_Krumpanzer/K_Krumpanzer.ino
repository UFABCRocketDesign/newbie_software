#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float alt_inicial; 
float soma; 
int i;

float filtro(float med_movel) {
    float media = 0;
    int qtd_media = 5;

    if (qtd_media == 0 or qtd_media == 35) {
      qtd_media = 5;
      media = 0;
    }

    media += (med_movel - media) / qtd_media++;

    return media;
}


void setup (){
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }


  Serial.print("Temperatura (°C) = \t");
  Serial.print("Pressão (Pa) = \t");
  Serial.print("Altitude (m) = \t");
  Serial.println("Altitude com filtro (m) =");

  for(i=0; i<5; i++){
    soma += bmp.readAltitude();
  } 
 alt_inicial = soma/5;

}
void loop (){

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");

  float altura = bmp.readAltitude() - alt_inicial;
  Serial.print(altura);
  Serial.print("\t");
  float altura_filtrada = filtro(altura);
  Serial.print(altura_filtrada);

  Serial.println();
  delay(10);
}
