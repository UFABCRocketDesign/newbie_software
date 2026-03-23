#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float t0;
float t1;
float si=0;
int ci=0;
float mi=0;
float s=0;
int c=0;
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
  c+= 1;
  if (c>=5) {
    m=s/5;
    
    Serial.print(bmp.readTemperature());
    Serial.print('\t');
    Serial.print(bmp.readPressure());
    Serial.print('\t');
    Serial.print(m-mi);
    Serial.print('\t');
    Serial.print(bmp.readSealevelPressure());
    Serial.print('\t');
    Serial.println(bmp.readAltitude(101500));
  c=0;
  s=0;
  }
}