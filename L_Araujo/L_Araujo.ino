//É o meu blink

#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}

void printa_linha() {
  for (int i = 0; i < 55; i++) {
    Serial.print("_");
  }
  Serial.println("");
}
  
void loop() {
    //Pisca rapido duas vezes para iniciar medição
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }

    printa_linha();
    Serial.println("| Temperatura |  Pressão  |  Altitude  |  Pressão a nível do mar |  Real Altitude |");
    printa_linha();
           
    //Mede temperatura
    Serial.print(bmp.readTemperature());
    Serial.print(" *C    |");

    Serial.print(bmp.readPressure());
    Serial.print(" Pa     |");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(bmp.readAltitude());
    Serial.print(" meters    |");

    Serial.print(bmp.readSealevelPressure());
    Serial.print(" Pa     |");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print(bmp.readAltitude(101500));
    Serial.print(" meters     |");
    
    Serial.println();
    printa_linha();
    
    delay(500);
}
