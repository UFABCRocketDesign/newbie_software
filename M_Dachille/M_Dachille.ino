#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float x;
float i = bmp.readAltitude();
float media;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  // Criando a variável

}

void loop() {

  // Fazer a média das altitudes 
  for (int i = 0; i < 15; x = i++) {
    media = x/15;
  }
  //Altitude
  Serial.print(bmp.readAltitude());
  Serial.println();
}
