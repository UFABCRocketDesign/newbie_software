#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);


  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Não foi possível encontrar um sensor BMP085 válido, verifique a fiação!");
    while (1) {}
  }
}
// the loop function runs over and over again forever
void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print("\t\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t\t");

  Serial.println(bmp.readAltitude());
  
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(500);                      // wait for a second
}

