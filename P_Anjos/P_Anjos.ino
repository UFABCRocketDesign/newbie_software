#include <Adafruit_BMP085.h>
/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

// the setup function runs once when you press reset or power the board

float H1;// Variável global - Não é ressetada a cada loop. Armazena o dado.
float Hmax;

Adafruit_BMP085 bmp;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  Serial.println("Temperature(*C)\tPressure(Pa)\tAltitude(m)\tPressure at sealevel(calculated)(Pa)\tReal altitude(m)");
}

// the loop function runs over and over again forever
void loop() {

    // Serial.print("Temperature = ");
    //Serial.print(bmp.readTemperature());
    //Serial.print("\t");
    
    // Serial.print("Pressure = ");
    //Serial.print(bmp.readPressure());
    //Serial.print("\t");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    // Serial.print("Altitude = ");
    H1=bmp.readAltitude();
    Serial.print(bmp.readAltitude());
    Serial.print("\t");

   //Serial.print("Pressure at sealevel (calculated) = ");
    //Serial.print(bmp.readSealevelPressure());
    //Serial.print("\t");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  // Serial.print("Real altitude = ");
   //Serial.print(bmp.readAltitude(101500));
   //Serial.print("\t");
    
    Serial.println();
   
    if(H1-bmp.readAltitude()>=3){
        digitalWrite(LED_BUILTIN, HIGH);   // A partir do momento que a diferença de altitude for acima de 3, provavelmente o foguete está descendo.
        Serial.print("\t");
        Serial.print("Descendo");
        Hmax=H1;
    }
    if(H1-bmp.readAltitude()<=0){
        Serial.print("\t");
        Serial.print("Subindo");
        Hmax=bmp.readAltitude();
    }
}
