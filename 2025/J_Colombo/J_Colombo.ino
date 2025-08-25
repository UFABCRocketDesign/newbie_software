// Sempre verificar antes de compilar
// Foguete não tem delay
// Serial print ln pula a linha

// Bibliotecas aqui
#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}

  Serial.print("Temperature (*C)  ");
  Serial.print("/t");
  Serial.print("Pressure (Pa)  ");
  Serial.print("/t");
  Serial.print("Altitude (m)  ");
  Serial.print("/t");
  Serial.print("Pressure at sealevel (calculated in Pa) = ");
  Serial.print("/t");
  Serial.println("Real altitude (m)  ");

  }
}
  
void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("/t");
    Serial.print(bmp.readPressure());
    Serial.print("/t");
    Serial.print(bmp.readAltitude());
    Serial.print("/t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("/t");
    Serial.println(bmp.readAltitude(101500));
}