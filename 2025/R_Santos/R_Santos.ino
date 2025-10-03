#include <Adafruit_BMP085.h>
#include <stdio.h>

Adafruit_BMP085 bmp;

float solo = 0;
int idx[3] = {};
float altitudeArray[3][10];
float mediaAlt[3] = {};
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
  Serial.println("Temperature(C)|Pressure(PA)|Altitude(Pa)|Pressure at Sea Level(PA)|Real Altitude(m)|Altura Atual(FILTRADA)(m)|Altura Atual(raw)(m)");
}

void loop() {
  float altitude = bmp.readAltitude() - solo;   //Essa parte do codigo vai tirar apartir da 11 interação o valor idx = 0| idx++ da array e colocar um novo
  somaAlt[0] -= altitudeArray[0][idx[0]];              //Retira o ultimo valor usado
  somaAlt[0] += altitude;                      //soma que vai ser feita todo ciclo para poder fazer a media
  altitudeArray[0][idx[0]] = altitude;
  mediaAlt[0] = somaAlt[0] / 10;       
  idx[0] = (idx[0] + 1) % 10;               //contador circular de interações no array (idx)
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
  Serial.print(mediaAlt[0]);
  Serial.print('\t');
  Serial.println(altitude);
}
