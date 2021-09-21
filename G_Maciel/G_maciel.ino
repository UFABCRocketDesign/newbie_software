#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float novaAlt=0.0;
float velhaAlt=0.0;
float media=0.0;
float h = 0.0;
float nova_h = 0;
float lista[] = {0,0,0,0,0,0,0,0,0,0};
float media_mov = 0;

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
    media =  media + bmp.readAltitude();
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
  for (int j=0; j<10; j++) {
    media_mov = media_mov + lista[j];
  }
  media_mov = media_mov/10;
  nova_h = (h + media_mov)/2;
  for (int k=0; k<9; k++) {
    lista[k] = lista[k+1];
  }
  lista[9] = h;
  media_mov = 0;
  Serial.print(h);
  Serial.print("\t");
  Serial.println(nova_h);
  
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
