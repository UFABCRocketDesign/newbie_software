#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;
float med_alt = 0; 
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  Serial.println("Temperature\tPressure\tAltitude\tPressure\tAltitude\t");
  for(int i =0;i<10;i++){
    med_alt += bmp.readAltitude();
  }
  med_alt /=10;
}

void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    
    Serial.print(bmp.readPressure()- med_alt);
    Serial.print("\t");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(bmp.readAltitude());
    Serial.print("\t");

    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print(bmp.readAltitude(101500));
    Serial.println("\t");

    delay(300);
}
