#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alt_zero = 0;              

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {           
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) { }               
  }
  float soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
                      
  }
  alt_zero = soma/10.0;
}

void loop() {
  Serial.print(bmp.readTemperature());       
  Serial.print('\t');
  Serial.print(bmp.readPressure());          
  Serial.print('\t');
  Serial.print(bmp.readAltitude() - alt_zero);
  Serial.print(bmp.readSealevelPressure());  
  Serial.print('\t');
  Serial.println();

}
