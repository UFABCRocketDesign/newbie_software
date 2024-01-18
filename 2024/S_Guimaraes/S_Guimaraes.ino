#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);

  Serial.print("Temperatura (C)\t");
  Serial.print("Pressão (Pa)\t");
  Serial.print("Altitude (m)\n");

  if (!bmp.begin()) {
    Serial.println();
    while (1) {}
  }
}

void loop() {
  // Exibe os dados em forma de tabela
  Serial.print(bmp.readTemperature());
  Serial.print("\t\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t\t");
  Serial.println(bmp.readAltitude());
  Serial.println();
}
