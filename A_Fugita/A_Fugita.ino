
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#define tam 10
#define zer 10
float zerar = 0;
float filtro[tam] = {};

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  
  for(int i = 0;i<zer;i++){ //deixando na altitude 0
    float valor = bmp.readAltitude();
    zerar+= valor;
  }
  zerar /= zer;
  
  //Serial.println("Temperature\tPressure\tAltitude\tPressure at sealevel (calculated)\tReal Altitude"); 
}


void loop() {
  float alt_med= 0;
  float f1 = bmp.readAltitude()- zerar;
  //Serial.print("Temperature = ");
  //Serial.print(bmp.readTemperature());
  //Serial.print("\t");
  //Serial.print("Pressure = ");
  //Serial.print(bmp.readPressure());
  //Serial.print("\t");

  for (int i=tam-1; i>0; i--){
    filtro[i] = filtro[i-1];
  }
  filtro[0] = f1;
  
  
  for (int i=0; i<tam; i++){
    alt_med += filtro[i];
  }
  alt_med /= tam;


  //Serial.print("Altitude = ");
  
  Serial.print(f1);
  Serial.print("\t");
  Serial.print(alt_med);
  Serial.print("\t");

  //Serial.print("Pressure at sealevel (calculated) = ");
  //Serial.print(bmp.readSealevelPressure());
  //Serial.print("\t");

  //Serial.print("Real altitude = ");
  //Serial.print(bmp.readAltitude(101500));
  Serial.println();
  
}
