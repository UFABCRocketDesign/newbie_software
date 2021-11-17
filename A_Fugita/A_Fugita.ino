
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

// the setup function runs once when you press reset or power the board

float media = 0;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  //pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  
  for(int i = 0;i<10;i++){
    float valor = bmp.readAltitude();
    media = media + valor;
  }
  media = media/10;
  
  //Serial.println("Temperature\tPressure\tAltitude\tPressure at sealevel (calculated)\tReal Altitude"); 
}

// the loop function runs over and over again forever
void loop() {
  //Serial.print("Temperature = ");
  //Serial.print(bmp.readTemperature());
  //Serial.print("\t");

  //Serial.print("Pressure = ");
  //Serial.print(bmp.readPressure());
  //Serial.print("\t");

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  //Serial.print("Altitude = ");
  
  Serial.print(bmp.readAltitude() - media);
  Serial.print("\t");

  //Serial.print("Pressure at sealevel (calculated) = ");
  //Serial.print(bmp.readSealevelPressure());
  //Serial.print("\t");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  //Serial.print("Real altitude = ");
  //Serial.print(bmp.readAltitude(101500));
  Serial.println();
  
}
