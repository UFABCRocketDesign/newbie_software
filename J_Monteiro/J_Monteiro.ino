#include <Adafruit_BMP085.h>

//========================================================
 #define n 2
Adafruit_BMP085 bmp;

//========================================================
//-----Variáveis Globais-----

 float altitude;         //Altitude inicial
 float relative_average;    //Média Relativa
 float  current_Altitude;  //Altitude Atual
 float vet[n] ;           //Vetor

//========================================================
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor");
    while (1) {}
    pinMode(LED_BUILTIN, OUTPUT);
  }
  Serial.print("Temperature =  \t");
  Serial.print(" Pa \t");
  Serial.print("  Pressure = \t");
  Serial.print(" Pa \t");
  Serial.print("  Pressure at sealevel (calculated) = \t");
  Serial.print(" Pa \t");
  Serial.print("  Real altitude = \t");
  Serial.println(" meters \t");
}

void loop() {

  current_Altitude = bmp.readAltitude();
  float altitude = 0; 
  

  Serial.print(bmp.readTemperature() );
  Serial.print('\t');

  Serial.print( bmp.readPressure()  );
  Serial.print('\t');
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print( current_Altitude);
  Serial.print('\t');

  Serial.print( bmp.readSealevelPressure() );
  Serial.print('\t');


  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.

  Serial.print( bmp.readAltitude(101500));
  Serial.print('\t');

  
    Serial.print(current_Altitude);
    
    
  for ( float i = 0;i > n ; i ++) {

   current_Altitude = bmp.readAltitude();
   altitude = altitude + current_Altitude;
}
   relative_average = altitude/n;
   Serial.print(relative_average);
   Serial.print('\t');
   
    if (relative_average  < current_Altitude) {

      Serial.print("Subida \t");
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

    }
    else {

      Serial.print("Descida \t");
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (LOW is the voltage level);

    }
  
        Serial.println();
        Serial.print('\t');

}

     

     
