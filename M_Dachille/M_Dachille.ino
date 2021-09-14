#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
    // Criando a variável 
  float primeira_altitude, segunda_altitude;
}

void loop() {

  primeira_altitude = bmp.readAltitude();
  // Comparando as variáveis
  if((primeira_altitude << segunda_altitude)
  digitalWrite(LED_BUILTIN, HIGH));
  
    //Altitude
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
    //Altitude real
  Serial.print(bmp.readAltitude(101500));                       

  
segunda_altitude = bmp.readAltitude();


  Serial.println();
}
