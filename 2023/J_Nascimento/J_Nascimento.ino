
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alt = 0;
float altV = 0;

void setup() {

    
    Serial.begin(115200);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    }
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    // showing the header
    Serial.println("Temperature( Cº)                Pressure (Pa)                 Altitude (Meters)                  RealAltitude (Pa)");
    
    alt = bmp.readAltitude();
}

// the loop function runs over and over again forever
void loop() {
  
    Serial.print(bmp.readTemperature());

    Serial.print("             ");
    Serial.print(bmp.readPressure());
    
    Serial.print("           ");
    altV = (bmp.readAltitude()) - alt;
    Serial.print(altV);

    //Serial.print("        ");
    //Serial.print(bmp.readSealevelPressure());

    //Serial.print("             ");
     
    Serial.println();
    delay(500);
    // wait for a second
}
