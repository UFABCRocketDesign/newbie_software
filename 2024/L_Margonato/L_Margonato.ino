#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.print("Temperature:");
  Serial.print("\t");
  Serial.print("Pressure:");
  Serial.print("\t");
  Serial.print("Altitude:");
  Serial.print("\t");
  Serial.print("Pressure at sealevel:");
  Serial.print("\t");
  Serial.println("Real altitude:");

}
  
void loop() {

  digitalWrite(LED_BUILTIN, HIGH); 
  delay(3000);                      
  digitalWrite(LED_BUILTIN, LOW);   
  delay(500);                      

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.println(bmp.readAltitude(101500));
    
  delay(500);
}