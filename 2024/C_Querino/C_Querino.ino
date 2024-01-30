#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alturainicial;
float altura=0;
int l;
float altufinal;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  
  }
 for (l=1;l<=20;l++)
  
  {
    alturainicial = alturainicial + bmp.readAltitude();
  }
    altura = (alturainicial/20);
  Serial.print("Temperatura\tpressão\tAltitude\tpressão em relação ao mar\taltitude real");
}

void loop(){
   
    Serial.print(bmp.readTemperature());
    Serial.print("\t");

    Serial.print(bmp.readPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude()-altura);
    Serial.print("\t");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude(101500));
    Serial.println();
    }



