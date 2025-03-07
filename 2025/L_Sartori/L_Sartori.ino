#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
#define N 3
#define L 5
#define H 4
bool ar;
bool h = false;
float med_alt = 0; 
float c[N][L];
float s[N+1];
float ss[3];
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
    s[0] = bmp.readAltitude()- med_alt;
    
    for (int i =0 ; i<N;i++){
      c[i][k]= s[i];
      s[i+1] =0;
      for(int j =0;j<L;j++){
        s[i+1]+= c[i][j];
      }
      s[i+1]/=5;
    }
    
    k+=1;
    k%=5;
    for(int i=0; i<N+1; i++){
      Serial.print(s[i]);
      Serial.print("\t");
    }
    
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

    for(int i=H-1;i>1;i--){
       ss[i] = s[i-1];
    }
    ss[0] = s[3];
    ar = true;
    for(int i=0;i<H-1;i++){
      ar = ar && ss[i]<ss[i+1];
      
    }
    h  = h || ar;
    
    Serial.print(h);
    Serial.print("\t"); 
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print((bmp.readAltitude(101500)));
    Serial.println("\t");

    h = false;


}
