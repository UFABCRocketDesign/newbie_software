#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float t0;
float t1;
float t2;
float t3;
float t4;
float t5;
float t6;
float t7;
float som_i=0;
int contador_i=0;
float med_i=0;
float som=0;
int contador=1;
float med=0;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
    Serial.print("Temperature\t");
    Serial.print("Pressure\t");
    Serial.print("Altitude\t");
    Serial.print("Pressure at sealevel (calculated)\t ");
    Serial.println("Real altitude");
    while(contador_i<30){
    t0=bmp.readAltitude();
    som_i=t0+som_i;
    contador_i+= 1;
  }
  med_i=som_i/30;
}
void loop() {
  t1=bmp.readAltitude();
    if(contador==2) {
      t2=t1;
    }
    else if (contador==3) {
      t3=t1;
    }
    else if (contador==4) {
      t4=t1;
    }
    else if (contador==5) {
      t5=t1;
    }
    else if (contador==6) {
      t6=t1;
    }
    else if (contador==7) {
      t7=t1;
    }
    else {
      t6=t7;
      t5=t6;
      t4=t5;
      t3=t4;
      t2=t3;
      t7=t1;
      med=(t6+t5+t4+t3+t2+t1)/6;
    }
    Serial.print(bmp.readTemperature());
    Serial.print('\t');
    Serial.print(bmp.readPressure());
    Serial.print('\t');
    Serial.print(med-med_i);
    Serial.print('\t');
    Serial.print(bmp.readSealevelPressure());
    Serial.print('\t');
    Serial.println(bmp.readAltitude(101500));
  contador+= 1;
}