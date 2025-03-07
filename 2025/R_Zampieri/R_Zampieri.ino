#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
//Declarando variáveis e array
float tara = 0;
float vetor[5];
int guia = 0;
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

  Serial.print("Temperatura\t");
  Serial.print("Pressão\t");
  Serial.print("Altitude Filtrada\t");
  Serial.print("Pressão no Nível do Mar\t");
  Serial.print("Altitude Real\t");
  Serial.print("Altitude Sem Filtro\t");
  Serial.println();
}

float altitude_filtrada = (vetor[0] + vetor[1] + vetor[2] + vetor[3] + vetor[4]) / 5;

void loop() {

  //atualização dos valores gravados
  vetor[guia] = bmp.readAltitude() - tara;
  if (guia < 4) {
    guia += 1;
  } else {
    guia = 0;
  }
  
  float altitude_filtrada = (vetor[0] + vetor[1] + vetor[2] + vetor[3] + vetor[4]) / 5;

  //print dos valores medidos
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(altitude_filtrada);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");
  Serial.print(vetor[guia]);
  Serial.print("\t");
  Serial.println();
}
