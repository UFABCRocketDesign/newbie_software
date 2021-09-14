#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float primeira_altitude, segunda_altitude;
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

  primeira_altitude = bmp.readAltitude();
  // Comparando as variáveis
  if (primeira_altitude > segunda_altitude){
    Serial.println("caindo");
  }
  //Altitude
  Serial.print(primeira_altitude);
  Serial.print("\t");
  segunda_altitude = primeira_altitude;
  Serial.println();
}
