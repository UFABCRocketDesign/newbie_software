#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

#define AMOSTRAS 20
float leituras[AMOSTRAS];
float soma = 0;
int indice = 0;
int indice1 = 0; 
float altitudeTarada = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
    while (1) {}
  }

  for (int i = 0; i < 10; i++) {
    altitudeTarada += bmp.readAltitude();
  }
  altitudeTarada /= 10.0;

  for (int i = 0; i < AMOSTRAS; i++) {
    float valor = bmp.readAltitude();
    leituras[i] = valor;
    soma += valor;
  }

  Serial.println("Temperatura\tPressao\tAltitude\tNivel do mar\tAltitude Filtrada\tAltura");
}

void loop() {
  float altitude = bmp.readAltitude();
  float altura = altitude - altitudeTarada ;
  soma = soma - leituras[indice] + altura;
  leituras[indice] = altura;
  indice = (indice + 1) % AMOSTRAS;

  float altitudeFiltrada = soma / AMOSTRAS;
  
  float soma2 = soma2 - leituras[indice1] + altitudeFiltrada;
  leituras[indice1] = altitudeFiltrada;
  indice1 = (indice1 + 1) % AMOSTRAS;
  float altitudeFiltrada2 = soma2/AMOSTRAS;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(altitude);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(altitudeFiltrada2);
  Serial.print("\t");
  Serial.println(altura);
}