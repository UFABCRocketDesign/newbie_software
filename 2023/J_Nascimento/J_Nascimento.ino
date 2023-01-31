
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
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("           ");
    Serial.print(bmp.readAltitude());

    Serial.print("        ");
    Serial.print(bmp.readSealevelPressure());

    // you can get a more precise measurement of altitude
    // if you know the current sea level pressure which will
    // vary with weather and such. If it is 1015 millibars
    // that is equal to 101500 Pascals.
    Serial.print("             ");
    altV = (bmp.readAltitude(101500)) - alt;
    Serial.print(altV); 
    //Serial.print(bmp.readAltitude(101500));
    
    Serial.println();
    delay(500);
    // wait for a second
}
