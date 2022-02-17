#include <Adafruit_BMP085.h>
#define led 13 
Adafruit_BMP085 bmp;

void setup() {
 pinMode(led, OUTPUT);
 Serial.begin(115200);
 if (!bmp.begin()) {
 Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  Serial.print("Temp (*C) |");
  Serial.print("Pres (Pa) |");
  Serial.print("Alt (m)|");
  Serial.println();
}

void loop() {
  
 Serial.print(bmp.readTemperature());
 Serial.print("\t  ");
 Serial.print(bmp.readPressure());
 Serial.print("\t  ");
 Serial.print(bmp.readAltitude());
 Serial.print("\t  ");
 Serial.println();
 
 
//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(500);
//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(500);
//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(500);
//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(2000);
  
}
