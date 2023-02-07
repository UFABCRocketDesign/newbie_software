#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float alt_inicial;
float alt; 
int i;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  
  Serial.println(" Temperature(*C) | Pressure(Pa) | Altitude(meters)");
  Serial.println();

   for(i=0; i<5; i++){
    alt += bmp.readAltitude();
  } 
 alt_inicial = alt/5;
}

void loop() {

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude() - alt_inicial);
    Serial.print("\t");


  Serial.println();
  delay(5);
}