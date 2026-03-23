#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float t0;
float t1;
float t2;
float t3;
float t4;
float t5;
float si=0;
int ci=0;
float mi=0;
float s=0;
int c=1;
float m=0;
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
    while(ci<30){
    t0=bmp.readAltitude();
    si=t0+si;
    ci+= 1;
  }
  mi=si/30;
}
void loop() {
  t1=bmp.readAltitude();
  s+= t1;
    if(c==1){
      s+=t1;
      m=s/c;
    }
    else if (c==2) {
      t2=t1;
      s+=t2;
      m=s/c;
    }
    else if (c==3) {
      t3=t1;
      s+=t3;
      m=s/c;
    }
    else if (c==4) {
      t4=t1;
      s+=t4;
      m=s/c;
    }
    else if (c==5) {
      t5=t1;
      s+=t5;
      m=s/c;
    }
    else {
      t4=t5;
      t3=t4;
      t2=t3;
      t5=t1;
      m=(t4+t3+t2+t1)/4;
    }
    Serial.print(bmp.readTemperature());
    Serial.print('\t');
    Serial.print(bmp.readPressure());
    Serial.print('\t');
    Serial.print(m-mi);
    Serial.print('\t');
    Serial.print(bmp.readSealevelPressure());
    Serial.print('\t');
    Serial.println(bmp.readAltitude(101500));
  c+= 1;
}