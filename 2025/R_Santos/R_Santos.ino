#include <Adafruit_BMP085.h>
#include <stdio.h>

Adafruit_BMP085 bmp;

float solo = 0;
int k = 0;
int K = 0;
float altitudeArray[10];
float mediaAlt = 0;
float somaAlt = 0;

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
  Serial.println("Temperature(C)|Pressure(PA)|Altitude(Pa)|Pressure at Sea Level(PA)|Real Altitude(m)|Altura Atual(m)");
}

void loop() {
  float altitude = bmp.readAltitude() - solo;   //Essa parte do codigo vai tirar apartir da 11 interação o valor k = 0| k++ da array e colocar um novo
  somaAlt -= altitudeArray[k];              //Retira o ultimo valor usado
  somaAlt += altitude;                      //soma que vai ser feita todo ciclo para poder fazer a media
  altitudeArray[k] += altitude;
  if(K < 10) K++;                           //contador que vai parar em 10 -> limite da array [10]
  mediaAlt = somaAlt / K;       
  k = k + 1 % 10;                           //contador circular de interações no array (k)
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");
  Serial.println(mediaAlt);
}
