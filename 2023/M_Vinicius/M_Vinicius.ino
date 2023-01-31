// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE)
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() {

  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin (115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
}

void loop() {

  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(3000);


  Serial.print ("Temperature (*C) =   ");
  Serial.print ("Pressure (Pa) =   ");
  Serial.print("Altitude (meters) =  ");
  Serial.println("Real altitude (meters) =   ");

  Serial.print(bmp.readTemperature());
  Serial.print("    ");


  Serial.print(bmp.readPressure());
  Serial.print("    ");

  Serial.print(bmp.readAltitude());
  Serial.print("    ");

  Serial.print(bmp.readAltitude(101500));
  Serial.print("    ");

  Serial.println();
    delay(500); 


}