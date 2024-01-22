#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float AltIni = 0;
int nLeitIni = 15;
float somaAltIni = 0;
float altitudes[10];
int frente = 0;
int nLeitFila = 10;
float rawAltitude = 0;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  if (!bmp.begin()) {

    Serial.println("Could not find a valid BMP085 sensor, check wiring!");

    while (1) {}
  }
  Serial.println("Temperature (*C) \t Pressure (Pa) \t Altitude (m) \t Pressure at sea level (Pa) \t Raw Altitude \t Real Altitude (m)");
  for (int i = 0; i < nLeitIni; i++) {
    somaAltIni += bmp.readAltitude();
  }
  AltIni = somaAltIni / nLeitIni;
  for (int i = 0; i < nLeitFila; i++) {
    altitudes[i] = bmp.readAltitude();
  }
}
void loop() {

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");

  rawAltitude = bmp.readAltitude() - AltIni;
  Serial.print(rawAltitude);
  Serial.print('\t');

  altitudes[frente] = rawAltitude;
  frente = (frente + 1) % nLeitFila;

  float somaAltRelativa = 0;

  for (int i = 0; i < nLeitFila; i++) {
    somaAltRelativa += altitudes[i];
  }
  Serial.println();
}