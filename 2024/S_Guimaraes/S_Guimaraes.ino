#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
 Serial.print("Temperatura (C)\t");
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.

  Serial.print("Pressão (Pa)\t");
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude (m)\n");
  if (!bmp.begin()) {
    Serial.println("Não foi possível encontrar um sensor BMP085 válido, verifique a conexão!");
    while (1) {}
  }
}
void loop() {
  // Exibe os dados em forma de tabela
Serial.print(bmp.readTemperature());
Serial.print("\t");
Serial.print(bmp.readPressure());
Serial.print("\t");
Serial.println(bmp.readAltitude(0));
Serial.println();
}
