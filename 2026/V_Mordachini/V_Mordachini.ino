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
  Serial.println(" *C");

  Serial.print("Pressao: ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print("Altitude: ");
  Serial.print(bmp.readAltitude());
  Serial.println(" m");
  Serial.println("--------------------");
  
  digitalWrite(LED_BUILTIN, HIGH); 
  Serial.print("LED LIGADO :)"); 
  delay(1500);                      
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("LED DESLIGADO :)");    
  delay(500);   
  
  digitalWrite(LED_BUILTIN, HIGH);  
  Serial.print("LED LIGADO :)");  
  delay(500);      
  digitalWrite(LED_BUILTIN, LOW); 
  Serial.print("LED DESLIGADO :)"); 
  delay(500);         
}
