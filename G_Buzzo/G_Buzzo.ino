#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

#define BuZZ (1)
#define BEEPING (BuZZ && 1)

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);     
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);

  digitalWrite(LED_BUILTIN, HIGH);   
  delay(700);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);   
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(700);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(700);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);

  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);     
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(100);

  Serial.print("Temperature = ");
  Serial.print("Pressure = ");
  Serial.print("Altitude = ");
  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.println("Real altitude = ");
  
  Serial.print(bmp.readTemperature());
  Serial.print(" ºC");
  Serial.print(bmp.readPressure());
  Serial.print(" Pa");
  Serial.print(bmp.readAltitude());
  Serial.print(" meters");
  Serial.print(bmp.readSealevelPressure());
  Serial.print(" Pa");
  Serial.print(bmp.readAltitude(101500));
  Serial.print(" meters");
    
  Serial.println();
  delay(500);
  
}
