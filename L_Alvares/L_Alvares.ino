#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  if (!bmp.begin()) 
  {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}


void loop() 
{
    Serial.print("Temperature= ");
    Serial.print(bmp.readTemperature());
    Serial.print(" *C ");
    Serial.print("Pressure= ");
    Serial.print(bmp.readPressure());
    Serial.print(" Pa ");
    Serial.print("Alt= ");
    Serial.print(bmp.readAltitude());
    Serial.print(" meters ");
    Serial.print("Pressure at sealevel (calculated)= ");
    Serial.print(bmp.readSealevelPressure());
    Serial.print(" Pa ");
    Serial.print("Real altitude= ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters ");
    Serial.println();
    
    digitalWrite(LED_BUILTIN, HIGH);   
    delay(2000);                       
    digitalWrite(LED_BUILTIN, LOW);    
    delay(1000); 
    
    delay(500);
}
