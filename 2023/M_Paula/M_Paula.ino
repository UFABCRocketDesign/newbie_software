#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#define n_media  5
#define num  50
float alt_inicial;
float values_1[num];
float values_2[num];
//----------------------------------------------------------------------
float media_movel_1(float sinal_com_ruido){
  float acc = 0;
  for(int i = num-1; i > 0; i--){
    values_1[i] = values_1[i-1];
  }
  values_1[0] = sinal_com_ruido;
  for(int i = 0; i < num; i++){
  acc += values_1[i];
  }
  return acc/num;
}
//----------------------------------------------------------------------
float media_movel_2(float sinal_sem_ruido){
  float acc = 0;
  for(int i = num-1; i > 0; i--){
    values_2[i] = values_2[i-1];
  }
  values_2[0] = sinal_sem_ruido;
  for(int i = 0; i < num; i++){
  acc += values_2[i];
  }
  return acc/num;
}
//----------------------------------------------------------------------
void setup (){
  float soma = 0;
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  for(int i = 0; i < n_media; ++i){
    soma = soma + bmp.readAltitude();
  } 
   alt_inicial = soma /5;
  Serial.print("Temperatura (*C) ");
  Serial.print("Altura com ruido (meters) ");
  Serial.print("Altura sem ruido (meters) ");
  Serial.println("Pressão (Pa)");
}
//----------------------------------------------------------------------
void loop (){
  //medicoes           
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    float altura_com_ruido = bmp.readAltitude() - alt_inicial;
    Serial.print(altura_com_ruido);
    Serial.print("\t");
    float altura_sem_ruido = media_movel_2(media_movel_1(altura_com_ruido));
    Serial.print(altura_sem_ruido);
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.println();
}