#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
float med_alt = 0; 
float c[5];
int j =0;
float s1;
int k;
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
  for(int i=0;i<5;i++){
    c[i]= bmp.readAltitude()- med_alt; 
  }
  s1 = (c[0]+c[1]+c[2]+c[3]+c[4])/5;
}

void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
   
    Serial.print(bmp.readPressure());
    Serial.print("\t");

    
    
    c[j] = bmp.readAltitude()- med_alt;
    float s2 = (c[0]+c[1]+c[2]+c[3]+c[4])/5;
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(c[j]);
    Serial.print("\t");
    
    Serial.print(s2);
    Serial.print("\t");
    
    j +=1;
    j%=5;
    
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
      
    if (s2-s1>= 0){
      Serial.print(0);
    } else {
      Serial.print(1); 
    }
    Serial.print("\t"); 
    s1 = s2;
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print((bmp.readAltitude(101500)));
    Serial.println("\t");




}
