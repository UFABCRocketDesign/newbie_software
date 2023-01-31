#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float alt_inicial;

void setup() {


  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  alt_inicial = 5;
  }

  Serial.println(" Temperature(*C) | Pressure(Pa) | Altitude(meters)");
  Serial.println();
}

void loop() {

  Serial.print(bmp.readTemperature());
  Serial.print("  ");
  Serial.print(bmp.readPressure());
  Serial.print("  ");
  Serial.print(bmp.readAltitude() - alt_inicial)
  
  ;
  Serial.print("  ");



  Serial.println();
  delay(5);
}