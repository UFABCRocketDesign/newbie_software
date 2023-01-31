#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float alt_inicial;
#define n_media  5
#define num  10
float values[num];
float media_movel(float sinal){
  int i;
  float acc = 0;
  values [0] = sinal;
  for(i = num; i > 0; i--){
    values [i] = values [i-1];
  }
  for(i = num; i < num; i++){
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
  Serial.print("Temperature (*C) ");
  Serial.print("Altitude com ruido (meters) ");
  Serial.print("Altitude sem ruido (meters) ");
  Serial.println("Pressure at sealevel (Pa)");
} 
//----------------------------------------------------------------------
void loop (){
  //medicoes           
    Serial.print(bmp.readTemperature());
    Serial.print("                 ");
    float altura_com_ruido = bmp.readAltitude() - alt_inicial;
    Serial.print(altura_com_ruido);
    Serial.print("                  ");
    float altura_sem_ruido = media_movel(bmp.readAltitude() - alt_inicial);
    Serial.print(altura_sem_ruido);
    Serial.print("                  ");
    Serial.print(bmp.readPressure());
    Serial.println();
}