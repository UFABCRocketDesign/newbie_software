#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#define n_media  5
float alt_inicial;
//----------------------------------------------------------------------
void setup (){
  float sinal[n_media];
  float soma = 0;
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  for(int i = 0; i < n_media; ++i){
    sinal[i] = bmp.readAltitude();
    soma = soma + sinal[i];
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
    Serial.print(bmp.readAltitude() - alt_inicial);
    Serial.print("                  ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println();
}