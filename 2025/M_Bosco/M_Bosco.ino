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
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(500);                      // wait for a second

    Serial.print("Temperatura = "); 
    Serial.print(bmp.readTemperature()); 
    Serial.println(" *C");
    
    Serial.print("Pressão = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" metros");

    Serial.print("Pressão ao nível do mar (calculada) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");


    Serial.print("Altitude real = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" metros");
    
    Serial.println();
}