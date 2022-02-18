#include <Adafruit_BMP085.h>
#define led 13 
Adafruit_BMP085 bmp;
float z, resultado, ta;

void setup() {
 pinMode(led, OUTPUT);
 Serial.begin(115200);
 if (!bmp.begin()) {
 Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  media();
  Serial.print("Temp (*C)\t");
  Serial.print("Pres (Pa)\t");
  Serial.print("Alt (m)\t");
  Serial.println();

}

void loop() {

ta = bmp.readAltitude() - resultado;
 
// Serial.print(bmp.readTemperature());
// Serial.print("\t");
// Serial.print(bmp.readPressure());
// Serial.print("\t");
 Serial.print(ta);
 Serial.print("\t");
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

void media() {
  z = 0;
 for(int x = 0; x < 100; x++){
   z = bmp.readAltitude() + z;
 }
 resultado = z/100;
}
