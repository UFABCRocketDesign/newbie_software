#include <Adafruit_BMP085.h>

//========================================================
#define n 25         // Número de medições para a média móvel
#define numReads 15 //Número de medições para a média da base
Adafruit_BMP085 bmp;

//========================================================
//-----Variáveis Globais-----

float altitude = 0;              //Altitude
float relative_average;         //Média Relativa
float current_Altitude;        //Altitude Atual
float vet[n] ;                //Vetor 1
float Altbase;               //Altitude no solo
float accAltbase = 0;       //Altitude inicial base acumulativa
float Height;              //Altura na base
float Maximum_height;     //Altura máxima
float previous;          //Altitude anterior
float moving_average;   //Média móvel
float vet2[n];         //Vetor 2
float value;          
float height;

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

  for (float k = 0; k < numReads; k++) {


    accAltbase = accAltbase + bmp.readAltitude();
  }
  Altbase = accAltbase / numReads;   //Média das medições do foguete na base
  Serial.print(Altbase);
  Serial.println('\t');
}

void loop() {

  float previous = 0;
  float Height = 0;
  float current_height;


  current_height = bmp.readAltitude() - Altbase; //Transformar altitude em altura
  altitude = 0;
  Maximum_height = 0;
  Serial.print(current_height);
  Serial.print('\t');


  //
  //  Serial.print(bmp.readTemperature() );
  //  Serial.print('\t');
  //
  //  Serial.print( bmp.readPressure()  );
  //  Serial.print('\t');
  //  // Calculate altitude assuming 'standard' barometric
  //  // pressure of 1013.25 millibar = 101325 Pascal
  //  Serial.print( current_Altitude);
  //  Serial.print('\t');
  //
  //  Serial.print( bmp.readSealevelPressure() );
  //  Serial.print('\t');
  //
  //
  //  // vary with weather and such. If it is 1015 millibars
  //  // that is equal to 101500 Pascals.
  //
  //  Serial.print( bmp.readAltitude(101500));
  //  Serial.print('\t');

  //
  //  Serial.print(current_high);
  //  Serial.print('\t');

  //Primeira camada de filtro

  for ( int i = n - 1; i > 0 ; i --) {

    vet[i]  =  vet [i - 1];

  }
  vet[0] = current_height;

  Height = 0;

  for ( int i = 0; i < n ; i ++) {

    Height = Height + vet [i];
  }
  relative_average = Height / n;
  Serial.print(relative_average);
  Serial.print('\t');

  //Segunda camada de Fitro

  for ( int i = n - 1; i > 0 ; i --) {

    vet2[i]  =  vet2 [i - 1];

  }
  vet2[0] = relative_average;

  Height = 0;

  for ( int i = 0; i < n ; i ++) {

    Height = Height + vet2[i];
  }
  moving_average = Height / n;
  Serial.print(moving_average);
  Serial.print('\t');
 
   height = current_height;
   value = moving_average - height;
  
  Serial.print(value);
  Serial.print('\t');
 
  if ( value < 0 ) {
      
  
    Serial.print("Subida \t");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
   }

   if ( value = 0) {

    Serial.print("Subida \t");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    
  }
  else {

    Serial.print("Descida \t");
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (LOW is the voltage level);

  }

  Serial.println();

}
