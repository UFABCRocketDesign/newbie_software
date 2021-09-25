#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float altitude1, altitude2, media2, media1;
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
  altitude1 = bmp.readAltitude();
  for (int i = 0; i < 10; i++) {
    media1 = media1 + altitude1;
  }

  //Fazer a média móvel
  media2 = media1 + altitude2;
  
  //calculo real da média
  media = media2/11;  
}

void loop() {
  //Altitude
  Serial.print(media);

  altitude2 = altitude1;
  Serial.println();
}
