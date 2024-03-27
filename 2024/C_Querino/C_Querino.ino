#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alturainicial;
float altura=0;
float height[19];
float altufinal;
float somaH;
float filtroA;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()){
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");}
  for (int i=0;i<20;i++){ 
    height[i] = bmp.readAltitude();
    somaH = height[i]+somaH;
    alturainicial = alturainicial + bmp.readAltitude();
  }
  filtroA = somaH/20;
  altura = (alturainicial/20);
  Serial.print("Temperatura\tpressão\tAltitude\tpressão em relação ao mar\taltitude real");
}
void loop(){
    for (int j=0;j<20;j++){ 
      height[j] = bmp.readAltitude();
      somaH = height[j]+somaH;
      filtroA = somaH/20;}
    
    Serial.print(bmp.readTemperature());
    Serial.print("\t");

    Serial.print(bmp.readPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude()-altura);
    Serial.print("\t");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude(101500));
    Serial.println();
    
    Serial.print(filtroA);
    Serial.println("filtro");
    
    somaH=0;
    }
