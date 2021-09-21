#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float novaAlt=0.0;
float velhaAlt=0.0;
float altitude=0.0;
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
  for (int i=0; i<20; i++) {
    altitude=bmp.readAltitude();
    media =  media + altitude;
  }
  media = media / 20;
  
  // Serial.println("Altitude [m]\tApogeu"); 

}

// the loop function runs over and over again forever
void loop() {
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  float novaAlt=bmp.readAltitude();
 
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
