// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE) 
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
  int i, w;
  float alt_in = 0;
  int count = 0;
  float altura_semRuido = 0;
  //float vetor[10];

  
  void setup() {

  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin (115200);

  Serial.print ("Temperature (*C) \t");
  Serial.print ("Pressure (Pa) \t");
  Serial.print("Altitude (meters) \t");
  Serial.print("Altitude sem ruido (meters) \t");
  

  
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
  
  if (count % 10 == 0 )
   {
     altura_semRuido = bmp.readAltitude()-alt_in;
   }
    else if (count == 0)
    {
        altura_semRuido = bmp.readAltitude()-alt_in;
    }
    
   Serial.print(altura_semRuido);
  Serial.print("\t");
  
  count++;
  
  Serial.println();
    delay(5); 

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

}