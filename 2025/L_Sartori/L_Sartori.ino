#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
bool h = false;
float med_alt = 0; 
float c[3][5];
int k=0;
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

}

void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
   
    Serial.print(bmp.readPressure());
    Serial.print("\t");

    c[0][k]=  bmp.readAltitude()- med_alt;
    for (int i =0 ; i<2;i++){
      for(int j =0;j<5;j++){
        c[i+1][k]+= c[i][j];
      }
      c[i+1][k]/=5;
    }
    
    k+=1;
    k%=5;

    Serial.print(c[0][k]);
    Serial.print("\t");
    
    Serial.print(c[2][k]);
    Serial.print("\t");
   
    
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    if (k==0){  
      h = c[1][4]>c[1][k];
    } else {
      h = c[1][k-1]>c[1][k];
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
