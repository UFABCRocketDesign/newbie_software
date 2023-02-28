#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
float p0 = 0;
int passo = 20;
int i;
int contador = 0;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  Serial.print("temperatura ");
  Serial.print("pressao  ");
  Serial.print("altitude  ");
  Serial.print("pressao no nvl do mar ");
  Serial.print("real altitude  ");
  // definindo condiçoes iniciais
  for (i = 1; i <= passo; i++) {
    p0 = bmp.readAltitude() + p0;
  }
  p0 = p0 / passo;
}

void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print("  ");

  Serial.print(bmp.readPressure());
  Serial.print("  ");

  float v[10]={};
  bmp.readAltitude() - p0;
  contador = contador + 1;
  float altitude = v[10] / contador;
  Serial.print(altitude);
  Serial.print("  ");

  Serial.println();

  delay(5);
}