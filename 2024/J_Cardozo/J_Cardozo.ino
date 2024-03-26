#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float var;
float alturaInicial;
const int numLeituras = 20;
float leituras[numLeituras];
float medias[numLeituras]; 
int indiceLeitura = 0;
int indiceMedia = 0; 
float soma = 0;
float somaMedias = 0; 
float media = 0;
float mediaDasMedias = 0;
const int historicoTamanho = 5;
float historico[historicoTamanho];
int indiceHistorico = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  for (int i = 0; i < numLeituras; i++) {
    soma += bmp.readAltitude();
  }

  alturaInicial = soma / numLeituras;
  soma = 0;

  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - alturaInicial;
    soma += leituras[i];
    medias[i] = 0;
  }

  for (int i = 0; i < historicoTamanho; i++) {
    historico[i] = 0;
  }

  Serial.print("Temperature(*C)\t");
  Serial.print("Pressure(Pa)\t");
  Serial.print("Altitude com primeiro filtro(m)\t");
  Serial.print("Altitude com segundo filtro(m)\t");
  Serial.print("Altitude sem filtro(m)\t");
  Serial.print("Status\t");

  Serial.println("");
}

void loop() {
  soma -= leituras[indiceLeitura];
  var = bmp.readAltitude() - alturaInicial;
  leituras[indiceLeitura] = var;
  soma += leituras[indiceLeitura];
  if (++indiceLeitura >= numLeituras) {
    indiceLeitura = 0;
  }

  media = soma / numLeituras;

  somaMedias -= medias[indiceMedia];
  medias[indiceMedia] = media;
  somaMedias += medias[indiceMedia];
  if (++indiceMedia >= numLeituras) {
    indiceMedia = 0;
  }

  mediaDasMedias = somaMedias / numLeituras;

  historico[indiceHistorico] = mediaDasMedias;
  if (++indiceHistorico >= historicoTamanho) {
    indiceHistorico = 0;
  }

  bool estaDescendo = true;
  for (int i = 0; i < historicoTamanho; i++) {
    if (mediaDasMedias >= historico[i]) {
      estaDescendo = false;
      break;
    }
  }

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(media);
  Serial.print("\t");
  Serial.print(mediaDasMedias); 
  Serial.print("\t");
  Serial.print(var);
  Serial.print("\t");

  if (estaDescendo) {
    Serial.print("Descendo");
  } else {
    Serial.print("Estável ou Subindo");
  }
  Serial.println();

}
