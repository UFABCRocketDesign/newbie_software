//É o meu blink

#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float m = 0;
int n = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  Serial.print("Altura  (metros)");

  float medida = bmp.readAltitude();
  m += medida;
  n++;
  
}
  
void loop() {
//    //Pisca rapido duas vezes para iniciar medição
//    for (int i = 0; i < 2; i++) {
//      digitalWrite(LED_BUILTIN, HIGH);
//      //delay(500);
//      digitalWrite(LED_BUILTIN, LOW);
//      //delay(500);
//    }
           
    //Mede temperatura
//    Serial.print(bmp.readTemperature());
//    Serial.print('\t');
//
//    Serial.print(bmp.readPressure());
//    Serial.print('\t');

    float medida = bmp.readAltitude();
    m += medida;
    n++;
    
    Serial.print(medida - (m/n));
    Serial.println('\t');

//    Serial.print(bmp.readSealevelPressure());
//    Serial.print('\t');

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
//    Serial.print(bmp.readAltitude(101500));
//    Serial.print('\t');
//    
    
    //delay(500);
}
