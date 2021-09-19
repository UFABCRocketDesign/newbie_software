//É o meu blink

#include <Adafruit_BMP085.h>

#define tam_vetor 100

Adafruit_BMP085 bmp;

float v[100] = {};
float zero = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  Serial.print("Altura  (metros)");

  for (int i = 0; i < tam_vetor; i++) {
    zero += bmp.readAltitude();
  }
  
  zero = zero/tam_vetor;
}

void loop() {
    float m = bmp.readAltitude() - zero;
    Serial.print(m);
    Serial.print('\t');

    for (int i = 99; i < tam_vetor; i++) {
      v[i - 1] = v[i];
    }
      v[99] = m;

    float aux = 0;
    for (int i = 0; i < tam_vetor; i++) {
      aux += v[i];
    }
    
    Serial.println(aux/tam_vetor);


}
