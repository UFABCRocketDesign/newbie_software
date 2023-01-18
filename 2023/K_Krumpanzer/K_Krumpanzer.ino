#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
void setup (){
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.print("Temperature  =     ");
  Serial.print("Pressure  =     ");
  Serial.println("Altitude  =     ");
}
void loop (){
  digitalWrite(13,HIGH);
  delay(2000);
  digitalWrite(13,LOW);
  delay(2000);


  Serial.print(bmp.readTemperature());
  Serial.println("     ");

  Serial.print(bmp.readSealevelPressure());
  Serial.println("     ");

  Serial.print(bmp.readAltitude());

}
