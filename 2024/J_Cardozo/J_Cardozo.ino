#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float alturaInicial;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  float somaAltura = 0;
  for (int i = 0; i < 10; i++) {
    somaAltura += bmp.readAltitude();
  }
  alturaInicial = somaAltura / 10;
  
  Serial.print("Temperature(*C)\t");
  Serial.print("Pressure(Pa)\t");
  Serial.print("Altitude(m)\t");
  

  Serial.println("");
}

void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude()-alturaInicial);
  Serial.print("\t");
  
  Serial.println();    
}
