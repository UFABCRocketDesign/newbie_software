// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE) 
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() {

  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin (115200);

  Serial.print ("Temperature (*C) \t");
  Serial.print ("Pressure (Pa) \t");
  Serial.print("Altitude (meters) \t");
  Serial.println("Real altitude (meters)");
  
  float alt_in = 0;

  for (i = 0, i<6, i++) {
    alt_in = alt_in + bmp.readAltitude();
  }

  alt_in = alt_in/6;

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}

    
  }
}

void loop() {

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

//
  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(bmp.readAltitude()-alt_in);
  Serial.print("\t");

  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");

  Serial.println();
    delay(100); 


}