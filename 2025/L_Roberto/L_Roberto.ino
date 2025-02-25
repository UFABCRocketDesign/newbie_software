#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
    Serial.println("\nTemperatura | Pressão | Altitude | Pressão Nivel do Mar | Altitude Real.");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
}
void loop(){
  int alt = 0;
  for (int i = 0; i < 100; i++) 
    alt += bmp.readAltitude();
  alt = alt/100;

  Serial.print(bmp.readTemperature());
    Serial.print("\t");
  Serial.print(bmp.readPressure());
    Serial.print("\t");
  Serial.print(bmp.readAltitude() - alt);
    Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\n");

}