#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

  float Alt = 0;
 
  void setup() {
// initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    if (!bmp.begin()) {
	  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	  while (1) {}
    }
  //cabeçalho
    Serial.print("Temp");
    Serial.print("\t");

    Serial.print("Pressão");
    Serial.print("\t");

    Serial.print("Altitude");
    Serial.print("\t");
  
    Serial.print("P.Nível do Mar");
    Serial.print("\t");

    Serial.print("Alt.Nível do mar");
    Serial.println();

  //variaveis
   for ( i <= 4 ; i++ ) 
   {Alt += bmp.readAltitude();
   }
  
  //contas 
   float M = Alt / 5;
}

 // the loop function runs over and over again forever
  void loop() {

 //Leitura de dados

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
  
    Serial.print(bmp.readPressure());
    Serial.print("\t");
  
    Serial.print(bmp.readAltitude());
    Serial.print("\t");
    
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
  
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
   
    Serial.println();
}