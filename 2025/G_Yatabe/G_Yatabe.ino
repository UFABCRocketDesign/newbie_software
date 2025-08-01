/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://docs.arduino.cc/hardware/

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/
*/
// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here
// the setup function runs once when you press reset or power the board

#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float Alt_zero = 0; 

float vetor[75];

void setup() {
// initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  float soma = 0 ; 
  for ( int i = 0 ; i < 10 ; i++) {
    soma += bmp.readAltitude();
  }
  Alt_zero = soma / 10;

}

// the loop function runs over and over again forever
void loop() {     
    float nova_alt = (bmp.readAltitude() - Alt_zero);
    for(int i = 74; i > 0; i--){
      vetor[i] = vetor[i-1];
    }

    vetor[0] = nova_alt;

    float soma = 0;
    for(int i = 0; i<75; i++) {
      soma += vetor[i];
    }
    float media = soma/75;

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal 
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    Serial.print(nova_alt);
    Serial.print("\t");
    Serial.print(media);
    Serial.print("\t");
    Serial.println();
    
}

    
   
    