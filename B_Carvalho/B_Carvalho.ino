#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alt_atual = 0;
float alt_anterior = 0;
float media = 0;
float leitura = 0;
float num_leitura = 0;

void setup() { 
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.print("Temperature(°C ) = ");
  Serial.print("\t");
  Serial.print("Pressure(Pa) = ");
  Serial.print("\t");
  Serial.print("Altitude(m) = ");
  Serial.print("Pressure at sealevel (calculated as Pa) = ");
  Serial.print("\t");
  Serial.print("Real altitude(m) = ");
  Serial.print("\t");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  if (leitura == 1){
      alt_atual = bmp.readTemperature();  
      if (num_leitura >= 1){
        media = (alt_atual - alt_anterior);
        Serial.print(media);
        Serial.print("\t");
      }
      leitura = 1;
      alt_anterior = alt_atual;
    }  
  leitura = 1;
  num_leitura += 1;
  
  //Serial.print("\t");
  //digitalWrite(LED_BUILTIN, LOW);

  //digitalWrite(LED_BUILTIN, HIGH);
  //Serial.print(bmp.readPressure());
  //Serial.print("\t");
  //digitalWrite(LED_BUILTIN, LOW);

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  //Serial.print(bmp.readAltitude());
  //Serial.print("\t");

  //Serial.print(bmp.readSealevelPressure());
  //Serial.print("\t");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  //Serial.print(bmp.readAltitude(101500));
  //Serial.print("\t");

  Serial.println();
  delay(500);
}
