#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

// constants won't change. Used here to set a pin number:
const int ledPin =  LED_BUILTIN;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 2000;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output: BLINK
  pinMode(ledPin, OUTPUT);
  // Parte do BMP
  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  Serial.println("Temperatura °C \t Pressão Pa \t Altitude metros \t Pressão ao nivel do mar Pa \t Altitude real metros");
}

void loop() {
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  // Parte do BMP//////////////////////////////////////////////////
    //Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    
    //Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    //Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.print("\t");

    //Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    //Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    
    Serial.println();
    delay(500);
}
