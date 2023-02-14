// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE) 
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
  int i, w;
  float alt_in = 0;
  float atura_atual = 0;
  //float vetor[10];
  
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

  for (i = 0; i<5; i++) {              
      alt_in = alt_in + bmp.readAltitude();
    }

    alt_in = alt_in/5;
}

void loop() {


  Serial.print(bmp.readTemperature());            
  Serial.print("\t");

//
  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(bmp.readAltitude()-alt_in);
  Serial.print("\t");

  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");
    
   /* for (i = 0; i<10; i++)
    {
      vetor[i] = bmp.readAltitude()-alt_in
      for (w=0; w<9; w++)
      {
        if (vetor[w+1]>vetor[w])
        {
          Serial.print("0");
        }
        else {
          Serial.print("1");
        }        
      }
    }*/

    

  Serial.println();
    delay(100); 


}