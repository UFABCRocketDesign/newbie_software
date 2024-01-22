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
  alturainicial = bmp.readAltitude();

  Serial.print("Temperatura\tpressão\tAltitude\tpressão em relação ao mar\taltitude real");
}


void loop(){
 /* for (l=1;l<=10;l++)
  
  {
    altura = altura + bmp.readAltitude();
  }
    altura = (altura/10)-alturainicial*/
    
    Serial.print(bmp.readTemperature());
    Serial.print("\t");

    Serial.print(bmp.readPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude()-alturainicial);
    Serial.print("\t");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude(101500));
    Serial.println();
    }



