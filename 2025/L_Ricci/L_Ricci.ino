#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

const int numLeituras = 10;
float alt = 0;
float total = 0;
float leituras[numLeituras];
int indiceAtual = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.println("T\tP\tA\tPMar\tAReal\tAReal2");
  for (int i = 0; i < 150; i++) {
    alt += bmp.readAltitude();
  }
  alt = alt / 150;

  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = 0;
  }
}

void loop() {

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");

  total = total - leituras[indiceAtual];
  leituras[indiceAtual] = (bmp.readAltitude() - alt);
  total = total + leituras[indiceAtual];
  indiceAtual = (indiceAtual + 1) % numLeituras;
  float media = total / numLeituras;
  Serial.print(media);

  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.println();
  
}
