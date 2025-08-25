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
  }
}
  
void loop() {
    Serial.print("Temperature (*C)  ");
    Serial.print("Pressure (Pa)  ");
    Serial.print("Altitude (m)  ");
    Serial.print("Pressure at sealevel (calculated in Pa) = ");
    Serial.println("Real altitude (m)  ");

    Serial.print(bmp.readTemperature());
    Serial.print(bmp.readPressure());
    Serial.print(bmp.readAltitude());
    Serial.print(bmp.readSealevelPressure());
    Serial.println(bmp.readAltitude(101500));
}