#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  Serial.println("Temperature [*C] - Pressure [Pa] - Altitude [m] - Pressure at sealevel (calculated) [Pa] - Real altitude [m]"); 
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  Serial.print(bmp.readTemperature());
  Serial.print("-");
    
  Serial.print(bmp.readPressure());
  Serial.print("-");
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print(bmp.readAltitude());
  Serial.print("-");

  Serial.print(bmp.readSealevelPressure());
  Serial.print("-");

  Serial.print(bmp.readAltitude(101500));
  Serial.print("-");

  Serial.println();
  delay(1000);

}
