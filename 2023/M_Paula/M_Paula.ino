#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
void setup (){
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.print("Temperature (*C)   ");
  Serial.print("Altitude (meters)  ");
  Serial.println("Pressure at sealevel (Pa)  ");
}
void loop (){
  digitalWrite(13,HIGH);
  delay(3000);
  digitalWrite(13,LOW);
  delay(2000);
  //medicoes           
    Serial.print(bmp.readTemperature());
    Serial.print("    ");
    Serial.print(bmp.readAltitude());
    Serial.print("    ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println();
}