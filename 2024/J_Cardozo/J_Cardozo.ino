#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float pressaoInicial;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  pressaoInicial = bmp.readPressure();
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
  Serial.print(bmp.readAltitude(pressaoInicial)); //Calcula a altura utilizando a pressao calculada quando o setup inicia
  Serial.print("\t");
  
  Serial.println();    
}
