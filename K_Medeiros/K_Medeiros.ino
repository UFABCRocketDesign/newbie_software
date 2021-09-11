// the setup function runs once when you press reset or power the board

#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

   Serial.begin(9600);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    }

    Serial.print("Temperature ");
    Serial.print(" Pressure ");
    Serial.print(" Altitude  ");
    Serial.print(" Pressure at sealevel (calculated) = ");
    Serial.println(" Real altitude  ");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  
  Serial.print(bmp.readTemperature());
  //Serial.print(" *C");
  
  
  Serial.print(bmp.readPressure());
  Serial.print(" ");
  //Serial.print(" Pa");
    
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  
  Serial.print(bmp.readAltitude());
  Serial.print(" ");
  //Serial.print(" meters");

  
  Serial.print(bmp.readSealevelPressure());
  Serial.print(" ");
  //Serial.print(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    
  Serial.print(bmp.readAltitude(101500));
  //Serial.print(" meters");
  
  Serial.println();
  delay(500);
}
