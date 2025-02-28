#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
bool h = false;
float med_alt = 0; 
float c[5][2];
float ss1;
int j =1;
int jr =0;
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
    c[i][0]= bmp.readAltitude()- med_alt; 
    c[i][1]= 0;
  }
  s1 = (c[0][0]+c[1][0]+c[2][0]+c[3][0]+c[4][0])/5;
}

void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
   
    Serial.print(bmp.readPressure());
    Serial.print("\t");

    
    
    c[j][0] = bmp.readAltitude()- med_alt;
    s1 = (c[0][0]+c[1][0]+c[2][0]+c[3][0]+c[4][0])/5;
    c[j][1] = s1;
    ss1 = (c[0][1]+c[1][1]+c[2][1]+c[3][1]+c[4][1])/5;
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(c[j][0]);
    Serial.print("\t");
    
    Serial.print(c[j][1]);
    Serial.print("\t");
    
    j +=1;
    j%=5;
    jr +=1;
    jr%=5;    
    
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
      
    if (c[jr][1]>c[j][1]){
      h = true;
    } else {
      h = false; 
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
