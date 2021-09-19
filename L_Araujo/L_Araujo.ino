//É o meu blink

#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void printa_linha() {
  for (int i = 0; i < 70; i++) {
    Serial.print("_");
  }
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  printa_linha();
  Serial.println("| Temperatura (°C) |  Pressão (Pa)  |  Altitude  (meters) |  Pressão a nível do mar (Pa) |  Real Altitude (meters)|");
  printa_linha();
  
}
  
void loop() {
    //Pisca rapido duas vezes para iniciar medição
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
           
    //Mede temperatura
    Serial.print(bmp.readTemperature());
    Serial.print("|");

    Serial.print(bmp.readPressure());
    Serial.print("|");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(bmp.readAltitude());
    Serial.print("|");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("|");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print(bmp.readAltitude(101500));
    Serial.print("|");
    
    Serial.println();
    
    delay(500);
}
