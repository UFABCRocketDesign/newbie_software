#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float alt_inicial; 
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
    alt_inicial += bmp.readAltitude();
  } 
 
}
void loop (){

  Serial.print(bmp.readTemperature()-alt_inicial);
  Serial.print("     ");

  Serial.print(bmp.readSealevelPressure());
  Serial.print("     ");

  Serial.print(bmp.readAltitude());

  Serial.println();
  delay(10);
}
