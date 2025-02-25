#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
float med_alt = 0; 
float alt5;
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  Serial.println("Temperature\tPressure\tAltitude\tPressure\tAltitude\t");
  for(int i =0;i<10;i++){
    med_alt += bmp.readAltitude();
  }
  med_alt /=10;
  alt5 = 5 * med_alt; 
}

void loop() {
    float a = (bmp.readTemperature()+bmp.readTemperature()+bmp.readTemperature()+bmp.readTemperature()+bmp.readTemperature())/5;
    Serial.print(a);
    Serial.print("\t");
    float b =(bmp.readPressure()+bmp.readPressure()+bmp.readPressure()+bmp.readPressure()+bmp.readPressure())/5;
    Serial.print(b);
    Serial.print("\t");
    float c = (bmp.readAltitude()+bmp.readAltitude()+bmp.readAltitude()+bmp.readAltitude()+bmp.readAltitude()-alt5)/5;
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(c);
    Serial.print("\t");
    float d = (bmp.readSealevelPressure()+bmp.readSealevelPressure()+bmp.readSealevelPressure()+bmp.readSealevelPressure()+bmp.readSealevelPressure())/5;
    Serial.print(d);
    Serial.print("\t");
    float e = ((bmp.readAltitude(101500))+(bmp.readAltitude(101500))+(bmp.readAltitude(101500))+(bmp.readAltitude(101500))+(bmp.readAltitude(101500)))/5;
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print(e);
    Serial.println("\t");


}
