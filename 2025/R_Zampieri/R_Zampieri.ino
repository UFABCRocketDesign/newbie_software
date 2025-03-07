#include <Adafruit_BMP085.h>

#define tamanho 10

Adafruit_BMP085 bmp;
//Declarando variáveis e array
float tara = 0;
float vetor[tamanho];
float vetor2[tamanho];
int guia = 0;
float altitude_filtrada = 0;
float altitude_filtrada2 = 0;
//

void setup() {
  //Inicialização do sensor
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  //

  //Reset de altitude
  for (int i = 1; i <= 10; ++i) {
    tara += bmp.readAltitude();
  }
  tara /= 10;
  //

  //Serial.print("Temperatura\t");
  //Serial.print("Pressão\t");
  Serial.print("Altitude Filtrada\t");
  Serial.print("Altitude Filtrada 2\t");
  //Serial.print("Pressão no Nível do Mar\t");
  //Serial.print("Altitude Real\t");
  Serial.print("Altitude Sem Filtro\t");
  Serial.println();
}

void loop() {

  //atualização dos valores gravados
  vetor[guia] = bmp.readAltitude() - tara;
  if (guia < tamanho - 1) {
    guia += 1;
  } else {
    guia = 0;
  }

  for (int i = 0; i < tamanho; i += 1) {
    altitude_filtrada += vetor[i];
  }

  altitude_filtrada /= tamanho;

  vetor2[guia] = altitude_filtrada;
  if (guia < tamanho - 1) {
    guia += 1;
  } else {
    guia = 0;
  }

  for (int i = 0; i < tamanho; i += 1) {
    altitude_filtrada2 += vetor2[i];
  }
  altitude_filtrada2 /= tamanho;

  //print dos valores medidos
  //Serial.print(bmp.readTemperature());
  //Serial.print("\t");
  //Serial.print(bmp.readPressure());
  //Serial.print("\t");
  Serial.print(altitude_filtrada);
  Serial.print("\t");
  Serial.print(altitude_filtrada2);
  Serial.print("\t");
  //Serial.print(bmp.readSealevelPressure());
  //Serial.print("\t");
  //Serial.print(bmp.readAltitude(101500));
  //Serial.print("\t");
  Serial.print(vetor[guia]);
  Serial.print("\t");
  Serial.println();
}
