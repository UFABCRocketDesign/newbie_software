#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float altitudeInicial; //definiçao da altura inicial

//inicio de vetor e variavel acumulativa da altura relativa
//const int num = 5; 
//float Filtro[num];

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  altitudeInicial = bmp.readAltitude(); //definição da altura inicial
   
  Serial.println("*C\tPa\tmeters\tPa\tmeters");
}

void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
    //definição da altura relativa = 0
    float altitudeRelativa = bmp.readAltitude() - altitudeInicial; 
    Serial.print(altitudeRelativa); //print altura relativa

   //for (int i = 0; i < (num-1); i++){
    //Filtro[i]= altitudeRelativa
   //}
   


  Serial.print("\t");
  Serial.println();
}