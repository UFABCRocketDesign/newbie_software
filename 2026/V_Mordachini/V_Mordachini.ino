// Victor :)
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float altitudeInicial;
float alturaFiltrada = 0; // valor filtrado
const int medicoes = 30; // numero de leituras para calibragem
const float alpha = 0.1;   // fator de suavização

void setup() {
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  //LOGICA DE CALIBRACAO
  float soma = 0;
  for (int i = 0; i < medicoes; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma / medicoes; // altura 0
  
  // Inicializa o filtro com o valor zero
  alturaFiltrada = 0;

  //CABECALHO
  Serial.println("Temp(C)\tPressao(Pa)\tAlturaBruta(m)\tAlturaFiltrada(m)");
}

void loop() {
  // leitura bruta
  float altitudeAtual = bmp.readAltitude();
  float alturaBruta = altitudeAtual - altitudeInicial;

  // filtro EMA (media movel exponencial)
  // alturaFiltrada recebe 10% da leitura nova e mantém 90% da anterior
  alturaFiltrada = (alpha * alturaBruta) + (1.0 - alpha) * alturaFiltrada;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(alturaBruta);
  Serial.println("\t");

  Serial.print(alturaFiltrada);
  Serial.println("\t");
}