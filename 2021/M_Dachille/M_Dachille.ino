#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float zerado, filtro, m_filtro;
float media = 0;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  // Fazer a média das altitudes 
  for (int i = 0; i < 25; i++) {
    media = media + bmp.readAltitude();
  }
  //calculo real da média
  media = media/25;

  
}

void loop() {
  //"zerar" o valor
  zerado = bmp.readAltitude () - media;

  //filtro
  for (int j = 0; j < 10; j++) {
    filtro = filtro + zerado;
  }
  m_filtro = filtro/10;
  
  //Altitude
  Serial.print(zerado);
  Serial.print("\t");
  Serial.print(m_filtro);
  Serial.println();
}
