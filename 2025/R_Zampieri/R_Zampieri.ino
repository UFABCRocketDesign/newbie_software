#include <Adafruit_BMP085.h>

#define tamanho 10

Adafruit_BMP085 bmp;
//Declarando variáveis e array
float tara = 0;
float vetor[tamanho];
float vetor2[tamanho];
//float vetorqueda[2];
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
  //Serial.print("Temperatura\t");
  //Serial.print("Pressão\t");
  Serial.print("Altitude Sem Filtro\t");
  Serial.print("Altitude Filtrada\t");
  Serial.print("Altitude Filtrada 2\t");
  //Serial.print("Pressão no Nível do Mar\t");
  //Serial.print("Altitude Real\t");
  Serial.print("Detector de Queda\t");
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

  altitude_filtrada = 0;
  for (int i = 0; i < tamanho; i += 1) {
    altitude_filtrada += vetor[i];
  }

  altitude_filtrada /= tamanho;

  vetor2[guia2] = altitude_filtrada;
  if (guia2 < tamanho - 1) {
    guia2 += 1;
  } else {
    guia2 = 0;
  }

  altitude_filtrada2 = 0;
  for (int i = 0; i < tamanho; i += 1) {
    altitude_filtrada2 += vetor2[i];
  }
  altitude_filtrada2 /= tamanho;

  //SEGUNDA TENTATIVA DE DETECTOR DE QUEDA AQUI
  if (altitude_filtrada2 < alturapassada) {  //AQUI ELE COMPARA A ALTITUDE ATUAL COM A ANTERIOR
    detectorqueda = 1;
  } else {
    detectorqueda = 0;
  }
  alturapassada = altitude_filtrada2;  //AQUI ELE ARMAZENA A ALTITUDE ATUAL PRA USAR NO PRÓXIMO LOOP COMO alturapassada

  /*
  vetorqueda[alturapassada] = altitude_filtrada2;
  if (alturapassada < 1) {
    alturapassada += 1;
  } else {
    alturapassada = 0;
  }

  if (vetorqueda[0] < vetorqueda[1]) {
    detectorqueda = 1;
  } else {
    detectorqueda = 0;
  }
*/

  //print dos valores medidos
  //Serial.print(bmp.readTemperature());
  //Serial.print("\t");
  //Serial.print(bmp.readPressure());
  //Serial.print("\t");
  Serial.print(alturapassada);
  Serial.print("\t");  
  Serial.print(vetor[guia]);
  Serial.print("\t");
  Serial.print(altitude_filtrada);
  Serial.print("\t");
  Serial.print(altitude_filtrada2);
  Serial.print("\t");
  //Serial.print(bmp.readSealevelPressure());
  //Serial.print("\t");
  //Serial.print(bmp.readAltitude(101500));
  //Serial.print("\t");
  Serial.print(detectorqueda);
  Serial.print("\t");
  Serial.println();
}
