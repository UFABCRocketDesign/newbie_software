#include <Adafruit_BMP085.h>
#include <stdio.h>

Adafruit_BMP085 bmp;

float solo = 0;
int i = 0;
int idx[3] = {};
float alturaArray[3][10];
float mediaAlt[3+1] = {}; //+1 porque ele amazena a altura entrando
float somaAlt[3] = {};

void setup() {  ///inicialização
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for(int i= 0; i < 10; i++){
    solo += bmp.readAltitude();
  }
  solo = solo/10;

/// cabeçario
  Serial.println("Temperature(C)\tPressure(PA)\tReal Altitude(m)\tAltura Atual(raw)(m)\tAltura Atual(FILTRO 1)(m)\tAltura Atual(FILTRO 2)(m)\tAltura Atual(FILTRO 3)(m)");
}

void loop() {
  float altura = bmp.readAltitude() - solo;   //Essa parte do codigo vai tirar apartir da 11 interação o valor idx = 0| idx++ da array e colocar um novo
  mediaAlt[0] = altura;
  for(int i = 0; i<3; i++){
    somaAlt[i] -= alturaArray[i][idx[i]];              //Retira o ultimo valor usado
    somaAlt[i] += mediaAlt[i];                      //soma que vai ser feita todo ciclo para poder fazer a media
    alturaArray[i][idx[i]] = mediaAlt[i];
    mediaAlt[i+1] = somaAlt[i] / 10;       
    idx[i] = (idx[i] + 1) % 10;               //contador circular de interações no array (idx)
  }
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  for(int j = 0; j<4; j++){
    Serial.print(mediaAlt[j]);
    Serial.print("\t");
  }
  Serial.println();
}
