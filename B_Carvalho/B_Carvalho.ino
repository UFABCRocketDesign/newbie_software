#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Temperature(°C ) = ");
  Serial.println(" ");
  Serial.println("Pressure(Pa) = ");
  Serial.println(" ");
  Serial.println("Altitude(m) = ");
  Serial.print("Pressure at sealevel (calculated as Pa) = ");
  Serial.println(" ");
  Serial.print("Real altitude(m) = ");
  Serial.println(" ");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print(bmp.readTemperature());
  Serial.println(" ");
  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print(bmp.readPressure());
  Serial.println(" ");
  digitalWrite(LED_BUILTIN, LOW);

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print(bmp.readAltitude());
  Serial.println(" ");

  Serial.print(bmp.readSealevelPressure());
  Serial.println(" ");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  Serial.print(bmp.readAltitude(101500));
  Serial.println(" ");

  Serial.println();
  delay(500);
}
