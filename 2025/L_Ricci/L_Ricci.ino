#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

const int numLeituras = 15;
float alt = 0;
float total = 0;
float leituras[numLeituras];
float altitudeAnterior = 0;
int indiceAtual = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.println("Temperatura\tPressão\tAltitude Filtrada\tAltitude Raw\tPressão Mar\tPressão Local (hPa)\tQueda");
  for (int i = 0; i < 150; i++) {
    alt += bmp.readAltitude();
  }
  alt = alt / 150;

  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = 0;
  }
}

void loop() {
  float altitudeReal = bmp.readAltitude() - alt;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");

  total = total - leituras[indiceAtual];
  leituras[indiceAtual] = (altitudeReal);
  total = total + leituras[indiceAtual];
  indiceAtual = (indiceAtual + 1) % numLeituras;
  float media = total / numLeituras;
  Serial.print(media);

  Serial.print("\t");
  Serial.print(altitudeReal);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");

  if (altitudeReal < altitudeAnterior) {
    Serial.print(0);
  } else {
    Serial.print(1);
  }
  altitudeReal = altitudeAnterior;

  Serial.println();
}
