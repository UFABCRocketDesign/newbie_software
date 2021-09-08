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
  delay(200);      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);     
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);

  digitalWrite(LED_BUILTIN, HIGH);   
  delay(800);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);   
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(800);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(800);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);

  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);     
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);

  Serial.print("Temperatura "),
  Serial.print("Pressão "),
  Serial.print("Altitude "),
  Serial.print("Pressão nível do mar "),
  Serial.println("Altitude Real ");
  
  Serial.print(bmp.readTemperature()),
  Serial.print(" ºC"),
  Serial.print(bmp.readPressure()),
  Serial.print(" Pa"),
  Serial.print(bmp.readAltitude()),
  Serial.print(" metros"),
  Serial.print(bmp.readSealevelPressure()),
  Serial.print(" Pa"),
  Serial.print(bmp.readAltitude(101500)),
  Serial.print(" metros ");
    
  Serial.println();
  delay(500);
  
}
