#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.print("Temperature(°C ) = ");
  Serial.print(" ");
  Serial.print("Pressure(Pa) = ");
  Serial.print(" ");
  Serial.print("Altitude(m) = ");
  Serial.print("Pressure at sealevel (calculated as Pa) = ");
  Serial.print(" ");
  Serial.print("Real altitude(m) = ");
  Serial.print(" ");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print(bmp.readTemperature());
  Serial.print(" ");
  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print(bmp.readPressure());
  Serial.print(" ");
  digitalWrite(LED_BUILTIN, LOW);

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print(bmp.readAltitude());
  Serial.print(" ");

  Serial.print(bmp.readSealevelPressure());
  Serial.print(" ");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  Serial.print(bmp.readAltitude(101500));
  Serial.print(" ");

  Serial.println();
  delay(500);
}
