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
}

void loop() {

  Serial.print("Temperatura: ");
  Serial.print(bmp.readTemperature());
  Serial.print(" *C ||");

  Serial.print("Pressao: ");
  Serial.print(bmp.readPressure());
  Serial.print(" Pa ||");

  Serial.print("Altitude: ");
  Serial.print(bmp.readAltitude());
  Serial.print(" m ||");
  
  digitalWrite(LED_BUILTIN, HIGH); 
  Serial.print("LED PISCANDO :)"); 
  Serial.println("--------------------");
  delay(1500);                      
  digitalWrite(LED_BUILTIN, LOW); 
  delay(500);   
  
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(500);      
  digitalWrite(LED_BUILTIN, LOW); 
  delay(500);         
}
