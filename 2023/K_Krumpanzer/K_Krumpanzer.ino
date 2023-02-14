#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

#define n 10
#define n1 5

float alt_inicial; 
float soma; 
int i;
float num[n];
float num2[n];
float num3[n];

float filtro(float mediamovel){
  for(i = n-1; i>0; i--) num[i] = num[i-1];
  num[0] = mediamovel;
  float acc = 0;
  for(i=0; i<n; i++) acc += num[i];

  return acc/n;
}

float filtro2(float mediamovel2){
  for(i = n-1; i>0; i--) num2[i] = num2[i-1];
  num2[0] = mediamovel2;
  float acc2 = 0;
  for(i=0; i<n; i++) acc2 += num2[i];

  return acc2/n;
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
  Serial.println("Altitude com filtro (m) = \t");
  Serial.println("Altitude com filtro 2 (m) =");

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
  Serial.print("\t");
  float altura_filtrada2 = filtro2(altura_filtrada);
  Serial.print(altura_filtrada2);
  Serial.print("\t");  


  for(i = n1-1; i>0; i--)num3[i] = num3[i-1];
  num3[0] = altura_filtrada2;

  if(num3[0]>num3[1]>num3[2]>num3[3]>num3[4]){
  Serial.print("0");
  }else{
    Serial.print("1");
  }


  Serial.println();
  delay(10);
}

