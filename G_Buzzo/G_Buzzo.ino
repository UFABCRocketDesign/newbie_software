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

  Serial.print("Temperatura (ºC)");
  Serial.print("\t");
  Serial.print("Pressão (Pa)");
  Serial.print("\t");
  Serial.print("Altitude (m)");
  Serial.print("\t");
  Serial.print("Pressão nível do mar (Pa)");
  Serial.print("\t");
  Serial.println("Altitude Real (m)");
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


  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));

    
  Serial.println();
  delay(500);
  
}
