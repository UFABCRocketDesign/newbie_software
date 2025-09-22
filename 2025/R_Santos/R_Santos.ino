#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float solo = 0;
float mediaA = 0;

void setup() {  ///inicialização
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for(int i= 0; i < 10; i++){
    solo = bmp.readAltitude() + solo;
  }
  solo = solo/10;

/// cabeçario
  Serial.println("Temperature(C)|Pressure(PA)|Altitude(Pa)|Pressure at Sea Level(PA)|Real Altitude(m)|Altura Atual(m)");
}

void loop() {  ///leituras
  mediaA = 0;
  for(int i= 0; i < 20; i++){
    float altitude = bmp.readAltitude() - solo;
    mediaA = altitude + mediaA;
  }
  mediaA = mediaA/20;
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");
  Serial.println(mediaA);
}
