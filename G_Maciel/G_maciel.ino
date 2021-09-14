#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float novaAlt=0.0;
float velhaAlt=0.0;
float i=0.0;
float media=0.0;
float h = 0.0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  // pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  
  // Serial.println("Altitude [m]\tApogeu"); 

}

// the loop function runs over and over again forever
void loop() {
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  float novaAlt=bmp.readAltitude();
 

  if (i<20) {
    media =  media + novaAlt;
    i = i + 1.0;
  }
  if (i=20) {
    media = media/20;
    i = i + 1.0;
  }
  if (i>20) {
    h = novaAlt - media;
    Serial.println(h);
    // Serial.print("\t");
    //if (h < velhaAlt) {
      //Serial.println("caindo");
      //velhaAlt = h;
    //}
    //if (h > velhaAlt) {
      //Serial.println("subindo");
      //velhaAlt = h;
    //}
  }
}
