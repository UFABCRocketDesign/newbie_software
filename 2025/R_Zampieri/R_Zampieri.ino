#include <Adafruit_BMP085.h>

#define tamanho 10

Adafruit_BMP085 bmp;

//Declarando variáveis e arrays
float tara = 0;
float vetor[tamanho];
float vetor2[tamanho];
int guia = 0;
int guia2 = 0;
int detectorqueda = 0;
float alturapassada = 0;
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

  Serial.print("alturapassada\t");
  Serial.print("Altitude Sem Filtro\t");
  Serial.print("Altitude Filtrada\t");
  Serial.print("Altitude Filtrada 2\t");
  Serial.print("Detector de Queda\t");
  Serial.print("Temperatura\t");
  Serial.print("Pressão\t");
  Serial.print("Pressão no Nível do Mar\t");
  Serial.print("Altitude Real\t");
  Serial.println();
}

void loop() {

  //FILTROS
  vetor[guia] = bmp.readAltitude() - tara;  //setup do filtro 1
  if (guia < tamanho - 1) {
    guia += 1;
  } else {
    guia = 0;
  }

  altitude_filtrada = 0;                  //reset da altitude pra usar no filtro 1
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 1
    altitude_filtrada += vetor[i];
  }

  altitude_filtrada /= tamanho;  //output do filtro 1

  vetor2[guia2] = altitude_filtrada;  //setup do filtro 2
  if (guia2 < tamanho - 1) {
    guia2 += 1;
  } else {
    guia2 = 0;
  }

  altitude_filtrada2 = 0;                 //reset da altitude pra usar no filtro 2
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 2
    altitude_filtrada2 += vetor2[i];
  }
  altitude_filtrada2 /= tamanho;  //output do filtro 2

  //DETECTOR DE QUEDA
  if (altitude_filtrada2 < alturapassada) {  //Comparação da altitude atual pós-filtros com a altitude anterior ("alturapassada")
    detectorqueda = 1;
  } else {
    detectorqueda = 0;
  }
  alturapassada = altitude_filtrada2;  //Armazenamento da altitude atual para usar como "alturapassada" no próximo loop

  //print dos valores medidos
  Serial.print(alturapassada);
  Serial.print("\t");
  Serial.print(vetor[guia]);
  Serial.print("\t");
  Serial.print(altitude_filtrada);
  Serial.print("\t");
  Serial.print(altitude_filtrada2);
  Serial.print("\t");
  Serial.print(detectorqueda);
  Serial.print("\t");
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");
  Serial.println();
}
