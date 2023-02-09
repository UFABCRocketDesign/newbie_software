// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE) 
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
  int w, i, soma;
  float alt_in = 0;
  float vetor[100];

void setup() {

  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin (115200);

  Serial.print ("Temperature (*C) \t");
  Serial.print ("Pressure (Pa) \t");
  Serial.print("Altitude (meters) \t");
  Serial.println("Real altitude (meters)");
   
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
   
  }
}

void loop() {

  // FILTRO DE ALTITUDE

    w = 0;
    soma = 0;

   while (w<100) {
     
    for (i = 0, i<5, i++) {
      alt_in = alt_in + bmp.readAltitude();
    }

    alt_in = alt_in/5;
    vetor[w] = alt_in;
    
    alt_in = 0;

    w++;

  }

  for (i=0, i<100, i++)
  {
    soma = soma + vetor[i];
  }

  alt_in = soma/100;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

//
  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(bmp.readAltitude()-alt_in);
  Serial.print("\t");

  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");

  Serial.println();
    delay(100); 


}