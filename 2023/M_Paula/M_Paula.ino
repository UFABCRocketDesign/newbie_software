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
  Serial.print("Altitude (meters) ");
  Serial.println("Pressure at sealevel (Pa)");
} 
//----------------------------------------------------------------------
void loop (){
  digitalWrite(13,HIGH);
  delay(3000);
  digitalWrite(13,LOW);
  delay(2000);
  //medicoes           
    Serial.print(bmp.readTemperature());
    Serial.print("                 ");
    Serial.print(media_movel(bmp.readAltitude() - alt_inicial));
    Serial.print("                  ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println();
}