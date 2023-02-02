#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#define n_media  5
#define num  50
float alt_inicial;
float values[num];
float media_movel(float sinal){
  float acc = 0;
  for(int j = num -1; j > 0;j--){
    values [j] = values [j-1];
    for(int i = num-1; i > 0; i--){
    values [i] = values [i-1];
    }
  }
  values [0] = sinal;
  for(int i = 0; i < num; i++){
    acc += values [i];
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
    float altura_sem_ruido = media_movel(altura_com_ruido);
    Serial.print(altura_sem_ruido);
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.println();
}