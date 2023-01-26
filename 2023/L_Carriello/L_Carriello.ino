#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
  
float alt_inicial; 
float soma;
int i;

void setup() {
  pinMode(13,OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}

  }
  Serial.print("Temperature(C) ");
  Serial.print("Altitude(M) ");
  Serial.print("Pressure at sealevel(Pa) ");
  Serial.println();

  for(i=0; i<10; i++){
    soma += bmp.readAltitude();
  }
 
  alt_inicial = soma/10;

}
  
void loop() {
    
    Serial.print(bmp.readTemperature());
    Serial.print("       ");    
    Serial.print(bmp.readAltitude() - alt_inicial);
    Serial.print("       ");  
    Serial.print(bmp.readSealevelPressure());
   
   
    Serial.println();
    delay(10);

}