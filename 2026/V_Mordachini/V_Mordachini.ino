// pisca_led Victor :)
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.println("Temp(C)\tPress(Pa)\tAlt(m)\tStatus_LED");
}

void loop() {

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  
  digitalWrite(LED_BUILTIN, HIGH); 
  Serial.println("PISCANDO :)"); 
  delay(1500);                      
  digitalWrite(LED_BUILTIN, LOW); 
  delay(500);   
  
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(500);      
  digitalWrite(LED_BUILTIN, LOW); 
  delay(500);         
}
