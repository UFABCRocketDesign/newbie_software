//É o meu blink

#include <Adafruit_BMP085.h>

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

  for (int i = 0; i < 100; i++) {
    zero += bmp.readAltitude();
  }
  
  zero = zero/100;
}

void loop() {
    float m = bmp.readAltitude() - zero;
    Serial.print(m/100);
    Serial.print('\t');

    for (int i = 1; i < 100; i++) {
      v[i - 1] = v[i];
    }
      v[99] = m;

    float aux = 0;
    for (int i = 0; i < 100; i++) {
      aux += v[i];
    }
    
    Serial.println(aux/100);


}
