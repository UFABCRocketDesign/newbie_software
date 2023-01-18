#include <Adafruit_BMP085.h>

/*************************************************** 
  This is an example for the BMP085 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391

  These pressure and temperature sensors use I2C to communicate, 2 pins
  are required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

Adafruit_BMP085 bmp;

int temperatura_i = bmp.readTemperature();
int pressao_i = bmp.readPressure();
int altitude_i = bmp.readAltitude();

void setup() {
  Serial.begin(115200);
  Serial.print("Temperature(*C)   Pressure(Pa)   Altitude(m)");
  Serial.println(' ');
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

}

void piscar(){
  digitalWrite(LED_BUILTIN, HIGH); 
}

void apagar(){
  digitalWrite(LED_BUILTIN, LOW); 
  delay(2000);    
}


void loop() {

    Serial.print(bmp.readTemperature()- temperatura_i);
    Serial.print("      ");
    Serial.print(bmp.readPressure()- pressao_i);
    Serial.print("      ");
    Serial.print(bmp.readAltitude()- altitude_i);
    Serial.println(" ");
  
  delay(1000);
//    piscar();
//    Serial.print("Temperature = ");
//    Serial.print(bmp.readTemperature());
//    Serial.println(" *C");
//    apagar();


//   piscar();
//    Serial.print("Pressure = ");
//    Serial.print(bmp.readPressure());
//    Serial.println(" Pa");
//    apagar();

    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    

//    piscar();
//    Serial.print("Altitude = ");
//    Serial.print(bmp.readAltitude());
//    Serial.println(" meters");
//    apagar();


//    piscar();
//    Serial.print("Pressure at sealevel (calculated) = ");
//    Serial.print(bmp.readSealevelPressure());
//    Serial.println(" Pa");
//    apagar();

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.

//    piscar();
//    Serial.print("Real altitude = ");
//    Serial.print(bmp.readAltitude(101500));
//    Serial.println(" meters");
//    apagar();


}
