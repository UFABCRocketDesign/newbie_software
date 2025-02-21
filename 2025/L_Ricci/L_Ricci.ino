#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

// Aqui você designa o output do LED
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}

// Tudo dentro desse statment ficará em loop
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  Serial.print("Temperatura = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressao = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" metros");

  Serial.print("Pressao a nivel do mar (calculada) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");

  Serial.print("Altitude real = ");
  Serial.print(bmp.readAltitude(93325));
  Serial.println(" metros");
  
  Serial.println();
  delay(500);
}