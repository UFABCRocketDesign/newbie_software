#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float alt_inicial; 
float b_alt; 
int i;

void setup (){
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }


  Serial.print("Temperature  =     ");
  Serial.print("Pressure  =     ");
  Serial.println("Altitude  =     ");

  for(i=0; i<5; i++){
    b_alt += bmp.readAltitude();
  } 
 alt_inicial = b_alt/5;
}
void loop (){

  Serial.print(bmp.readTemperature());
  Serial.print("     ");

  Serial.print(bmp.readSealevelPressure());
  Serial.print("     ");

  Serial.print(bmp.readAltitude()-alt_inicial);

  Serial.println();
  delay(10);
}
