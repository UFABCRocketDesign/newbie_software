#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
#define AMOSTRAS 20
float leituras[AMOSTRAS];
float soma = 0;
int indice = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    while (1) {}
  }
  for (int i = 0; i < AMOSTRAS; i++) {
    float valor = bmp.readAltitude();
    leituras[i] = valor;
    soma += valor;
  }
  Serial.println("Temperatura \t Pressao \t Altitude \t Nivel do mar \t Altitude Filtrada");
}

void loop() {
  float valor = bmp.readAltitude();
  soma = soma - leituras[indice] + valor;
  leituras[indice] = valor;
  indice = (indice + 1) % AMOSTRAS;
  float altitudeFiltrada = soma / AMOSTRAS;
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.println(altitudeFiltrada);
}
