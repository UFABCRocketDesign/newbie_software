#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
// --- Variáveis Globais ---
float alturaZero = 0;
// --- Variáveis do Filtro ---
float alturaFiltradaSegundaVez = 0.0;
const float ALPHA = 0.15;
float alturaFiltrada = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);


  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Não foi possível encontrar um sensor BMP085 válido, verifique a fiação!");
    while (1) {}
  }
  // --- Calibração da Altura Zero ---
Serial.println("calibrando altura");
float soma = 0;
for (int i = 0; i < 20; i++) {
  soma += bmp.readAltitude();
  }
  alturaZero = soma/20;
}

void loop() {
  // --- Leituras e Cálculos ---
  float Altitude = bmp.readAltitude();
  float Altura = Altitude - alturaZero;
  // --- Aplicação do Primeiro Filtro ---
  alturaFiltrada = (ALPHA * Altura) + (1 - ALPHA) * alturaFiltrada;
  // --- Aplicação do Segundo Filtro (em cascata) ---
  alturaFiltradaSegundaVez = (ALPHA * alturaFiltrada) + (1.0 - ALPHA) * alturaFiltradaSegundaVez;

// --- Prints dos valores ---
  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(Altitude);
  Serial.print("\t");
  
  Serial.print(Altura);
  Serial.print("\t");

  Serial.print(alturaFiltrada);
  Serial.print("\t");

  Serial.print(alturaFiltradaSegundaVez);
  Serial.println();
}

