#define LED_PISCA 13
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
  
void setup() {
  pinMode(LED_PISCA, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
	while (1) {}
  }
}
  
void loop() {
    Serial.print("Temperatura = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressao = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressao a nivel do mar = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Altitude real = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");
    
    Serial.println();
    delay(500);

  digitalWrite(LED_PISCA, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(180);                     // wait for a second
  digitalWrite(LED_PISCA, LOW);   // turn the LED off by making the voltage LOW
  delay(180);
  digitalWrite(LED_PISCA, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(50);
  digitalWrite(LED_PISCA, LOW);  // turn the LED off by making the voltage LOW
  delay(500);
  //comentario super original feito por cesar
}
