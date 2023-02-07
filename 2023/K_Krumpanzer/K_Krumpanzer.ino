#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

#define n 10

float alt_inicial; 
float soma; 
int i;
int num[n];

long filtro(float medmovel){
  for(i = n-1; i>0; i--) num[i] = num[i-1];
  num[0] = alt_inicial;
  medmovel = 0;
  for(i=0; i<n; i++) medmovel += num[i];

  return medmovel/n;
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
