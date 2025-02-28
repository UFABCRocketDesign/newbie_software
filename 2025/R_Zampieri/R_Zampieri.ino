#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float x = 0;
float filtro = 0;
float filtro_final = 0;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (int i = 1; i <= 10; ++i) {
    x = x + bmp.readAltitude();
    }
  x = x/10;
  Serial.print("Temperatura\t");
  Serial.print("Pressão\t");
  Serial.print("Altitude\t");
  Serial.print("Pressão no Nível do Mar\t");
  Serial.print("Altitude Real\t");
  Serial.println();
}

void loop() {
  for (int i = 1; i <= 5; ++i) {filtro = filtro + bmp.readAltitude();}
  filtro_final = filtro*5;
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(filtro_final-x);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");

  Serial.println();
}
