#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float var;
float alturaInicial;
const int numLeituras = 10;
float leituras[numLeituras];
int indiceLeitura = 0;
float soma = 0;
float media = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  // Calcula a altura inicial
  for (int i = 0; i < numLeituras; i++) {
    soma += bmp.readAltitude();
  }

  alturaInicial = soma / numLeituras;
  soma = 0; // Reseta a variavel soma para reutilizala

  // Inicializa o array de leituras com os valores do sensor
 for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - alturaInicial;
    soma += leituras[i];
  }

  Serial.print("Temperature(*C)\t");
  Serial.print("Pressure(Pa)\t");
  Serial.print("Altitude com filtro(m)\t");
  Serial.print("Altitude sem filtro(m)\t");

  Serial.println("");
}

void loop() {
  // Subtrai a última leitura
  soma -= leituras[indiceLeitura];
  // Lê a altitude atual e subtrai a altura inicial
  var = bmp.readAltitude() - alturaInicial;
  leituras[indiceLeitura] = var;
  // Adiciona a leitura atual à soma
  soma += leituras[indiceLeitura];
  // Avança para a próxima posição no array
  indiceLeitura++;

  // Se chegamos ao fim do array, volta para o inicio
  if (indiceLeitura >= numLeituras) {
    indiceLeitura = 0;
  }

  // Calcula a média
  media = soma / numLeituras;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(media);
  Serial.print("\t");
  Serial.print(var);
  Serial.print("\t");

  Serial.println();
}
