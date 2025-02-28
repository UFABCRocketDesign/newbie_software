#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
bool h = false;
float med_alt = 0; 
float c[2][5];
float f1;
float ff1;
int k;
int i;
void setup() {
  Serial.begin(115200);
 
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
    c[0][i]=0;
    c[1][i]=0; 
  }
}

void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
   
    Serial.print(bmp.readPressure());
    Serial.print("\t");

    
    for (i =0 ; i<5;i++){
      c[0][i]=  bmp.readAltitude()- med_alt;
      for(int j =0;j<5;j++){
        c[1][i]+= c[0][j];
      }
      c[0][i]/=5;
      for(int j =0;j<5;j++){
        ff1+= c[1][j];
      }
      ff1/=5;
    }
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(c[0][i]);
    Serial.print("\t");
    
    Serial.print(ff1);
    Serial.print("\t");
   
    
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    if (i==0){  
      h = c[1][5]>c[1][i];
    } else {
      h = c[1][i-1]>c[1][i];
    }
    Serial.print(h);
    Serial.print("\t"); 
    
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print((bmp.readAltitude(101500)));
    Serial.println("\t");




}
