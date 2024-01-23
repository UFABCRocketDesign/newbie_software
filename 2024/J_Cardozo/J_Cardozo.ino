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
    medias[i] = 0; // Inicializa o array de medias
  }

  Serial.print("Temperature(*C)\t");
  Serial.print("Pressure(Pa)\t");
  Serial.print("Altitude com primeiro filtro(m)\t");
  Serial.print("Altitude com segundo filtro(m)\t");
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

  // Armazena a média no array de medias e atualiza a soma de medias
  somaMedias -= medias[indiceMedia];
  medias[indiceMedia] = media;
  somaMedias += medias[indiceMedia];

  // Atualiza o índice para o array de medias
  indiceMedia++;
  if (indiceMedia >= numLeituras) {
    indiceMedia = 0;
  }

  // Calcula a média das médias
  mediaDasMedias = somaMedias / numLeituras;

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

  Serial.println();
}
