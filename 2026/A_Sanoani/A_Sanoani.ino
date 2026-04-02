#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float altitudeInicial;
float soma = 0;
int amostra = 100;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (int i = 0; i < amostra; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma / amostra;
  Serial.println("Temperature(*C)\tPressure(Pa)\tAltitude(meters)\tPressure at sealevel (calculated)(Pa)\tReal altitude(meters)");
  }


  void loop() {

    Serial.print(bmp.readTemperature());
    Serial.print("\t");

    Serial.print(bmp.readPressure());
    Serial.print("\t");

    Serial.print(bmp.readAltitude() - altitudeInicial);
    Serial.print("\t");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

    Serial.println(bmp.readAltitude(101500));
  }