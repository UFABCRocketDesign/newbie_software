#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float alt;
float leiturafiltrada = 0;

void setup(){  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
    Serial.println("\nTemperatura | Pressão | Altitude | Pressão Nivel do Mar | Altitude Real.");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  alt = 0;
  for (int i = 0; i < 100; i++) 
    alt += bmp.readAltitude();
  alt = alt/100 ;
}
  //leituraFiltrada = alpha * leituraAtual + (1 - alpha) * leituraFiltrada;
float filteredAltitude(){
  leiturafiltrada = (0.5*bmp.readAltitude()) + (0.5 * leiturafiltrada);
  return leiturafiltrada;
}
void loop(){
  Serial.print(bmp.readTemperature());
    Serial.print("\t");
  Serial.print(bmp.readPressure());
    Serial.print("\t");
  Serial.print(filteredAltitude());
    Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
    Serial.print("\n");
  

}