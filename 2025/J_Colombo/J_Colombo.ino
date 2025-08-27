// Sempre verificar antes de compilar
// Foguete não tem delay
// Serial println pula a linha
// Calcular altitude de referência

// Bibliotecas aqui
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float alturazero;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  float val_alts;

  for (int alt = 0;alt < 15; alt++) {
  val_alts = val_alts + bmp.readAltitude();
  } 

  alturazero = val_alts/15;

  Serial.print("Temperature (*C)  ");
  Serial.print("\t");
  Serial.print("Pressure (Pa)  ");
  Serial.print("\t");
  Serial.print("Altitude (m)  ");
  Serial.print("\t");
  Serial.print("Pressure at sealevel (calculated in Pa) = ");
  Serial.print("\t");
  Serial.print("Real altitude (m)  ");
  Serial.print("\t");
  Serial.println("Altitude zerada (m)  ");
}
  
void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    float altitude;
    altitude = bmp.readAltitude();
    Serial.print(altitude);
    Serial.print("\t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    float novaaltura;
    novaaltura = altitude- alturazero ;
    Serial.print(novaaltura);
    Serial.println();
}