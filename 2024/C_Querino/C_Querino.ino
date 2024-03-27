#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alturainicial;
float altura=0;
float height[20];
float altufinal;
float somaH;
float filtroA;
float altitude;
int j=0;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin())
  {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");}
  
  for (int i=0; i<20; i++){ 
    
    alturainicial = alturainicial + bmp.readAltitude();
  
  }

  altura = (alturainicial/20);
  Serial.print("Temperatura\tpressão\tAltitude\tpressão em relação ao mar\taltitude real");
}
void loop(){
    altitude = bmp.readAltitude()-altura;
    height[j] = altitude;
    j++;
    
    if (j>=20){
      j=0;}
    
    somaH=0;  
    
    for(int k=0; k<20; k++){
      somaH = somaH + height[k];
    }
    filtroA = somaH/20;
      
    Serial.print(bmp.readTemperature());
    Serial.print("\t");

    Serial.print(bmp.readPressure());
    Serial.print("\t");

    Serial.print(altitude);
    Serial.print("\t");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    
    Serial.print(filtroA);
    Serial.println();
    }
