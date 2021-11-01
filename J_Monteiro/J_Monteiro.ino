#include <Adafruit_BMP085.h>

//========================================================
#define n 25
#define numReads 15
Adafruit_BMP085 bmp;

//========================================================
//-----Variáveis Globais-----

float altitude = 0;              //Altitude
float relative_average;         //Média Relativa
float current_Altitude;       //Altitude Atual
float vet[n] ;               //Vetor
float Altbase;              // altitude no solo
float accAltbase = 0;      // altitude inicial base acumulativa
float High;               // Altura na base
float previous_altitude; //altitude anterior

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
  Altbase = accAltbase / numReads;
  Serial.print(Altbase);
  Serial.println('\t');
}

void loop() {

  float previous = 0;
  float High = 0;
  float current_high;

  current_high = bmp.readAltitude() - Altbase;
  altitude = 0;
  Serial.print(current_high);
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


  Serial.print(current_high);
  Serial.print('\t');


  for ( int i = n - 1; i > 0 ; i --) {

    vet[i]  =  vet [i - 1];

  }
  vet[0] = current_high;

  High = 0;

  for ( int i = 0; i < n ; i ++) {

    High = High + vet[i];
  }
  relative_average = High / n;
  Serial.print(relative_average);
  Serial.print('\t');


  if (relative_average < current_high ) {

    Serial.print("Subida \t");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  }
  else {

    Serial.print("Descida \t");
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (LOW is the voltage level);

  }

  Serial.println();

}
