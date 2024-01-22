#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float alturainicial;
float altura;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  
  }
  alturainicial = bmp.readRawTemperature();
  Serial.print("Temperatura\tpressão\tAltitude\tpressão em relação ao mar\taltitude real");
}


void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);                      
  digitalWrite(LED_BUILTIN, LOW);
  delay(501); 
  
    altura = (bmp.readTemperature() + alturainicial)/2;
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(altura);
    Serial.print("\t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    
    Serial.println();
    delay(500);                     
}