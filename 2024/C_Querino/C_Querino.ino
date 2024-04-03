#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alturainicial;
float altura = 0;
float height[20];
float altufinal;
float somaH;
float somaB;
float filtroA;
float filtroB[20];
float altitude;
float apojas = 0;
int n = 0;
int j = 0;
int r = 0;
int y = 1;
int apogeu = 0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  for (int i = 0; i < 20; i++) {

    alturainicial = alturainicial + bmp.readAltitude();
  }

  altura = (alturainicial / 20);
  Serial.print("Temperatura\tpressão\tAltitude\tpressão em relação ao mar\taltitude real");
}
void loop() {
  altitude = bmp.readAltitude() - altura;
  height[j] = altitude;
  j++;

  if (j >= 20) {
    j = 0;
  }

  somaH = 0;

  for (int k = 0; k < 20; k++) {
    somaH = somaH + height[k];
  }
  filtroA = somaH / 20;

  filtroB[r] = filtroA;
  r++;

  if (r >= 20) {
    r = 0;
  }

  somaB = 0;
  for (int v = 0; v < 20; v++) {
    somaB = somaB + filtroB[v];
  }
  float filtro = somaB / 20;


  if (apojas < filtro) {
    n++;
  } else {
    n = 0;
  }

  if (n>= 10) {
    apogeu = 1;
  }

  apojas = filtro;


  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(altitude);
  Serial.print("\t");

  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");

  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");

  Serial.print(filtroA);
  Serial.print("\t");

  Serial.print(filtro);
  Serial.print("\t");

  Serial.print(apogeu);
  Serial.println();
}
