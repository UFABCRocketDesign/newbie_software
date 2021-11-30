
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#define tam 20
#define zer 20
float zerar = 0;
float filtro[tam] = {};
float filtro1[tam] = {};

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
  
  int dec = 0;
  float f0 = bmp.readAltitude()- zerar;
  float f1 = 0; //saida do filtro0
  float f2 = 0; //saida do filtro1

  //Serial.print("Temperature = ");
  //Serial.print(bmp.readTemperature());
  //Serial.print("\t");
  //Serial.print("Pressure = ");
  //Serial.print(bmp.readPressure());
  //Serial.print("\t");

  for (int i=tam-1; i>0; i--){
    filtro[i] = filtro[i-1];
  }
  filtro[0] = f0;
  
  
  for (int i=0; i<tam; i++){
    f1 += filtro[i];
  }
  f1 /= tam;

  for (int i=tam-1; i>0; i--){ //filtro1
    filtro1[i] = filtro1[i-1];
  }
  filtro1[0] = f1;
  
  
  for (int i=0; i<tam; i++){
    f2 += filtro1[i];
  }
  f2 /= tam;

  if (bmp.readAltitude()> dec){
    Serial.print(1);
    Serial.print("\t");
    dec = 1;
  }
  else{
    Serial.print(0);
    Serial.print("\t");
    dec =0;
  }
  


  //Serial.print("Altitude = ");
  
  Serial.print(f0);
  Serial.print("\t");
  Serial.print(f1);
  Serial.print("\t");
  Serial.print(f2);
  Serial.print("\t");

  //Serial.print("Pressure at sealevel (calculated) = ");
  //Serial.print(bmp.readSealevelPressure());
  //Serial.print("\t");

  //Serial.print("Real altitude = ");
  //Serial.print(bmp.readAltitude(101500));
  Serial.println();
  
}
