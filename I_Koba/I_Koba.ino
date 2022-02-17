#include <Adafruit_BMP085.h>
#define led 13 
Adafruit_BMP085 bmp;

void setup() {
 pinMode(led, OUTPUT);
 Serial.begin(115200);
 Serial.print("Temp |");
 Serial.print("Pres |");
 Serial.print("Alt  |");
 Serial.println();
 
}

void loop() {
  
  Serial.print(bmp.readTemperature());
  Serial.print(" *C |");
  Serial.print(bmp.readPressure());
  Serial.print(" Pa |");
  Serial.print(bmp.readAltitude());
  Serial.print(" m |");
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
