#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float alt = 0;
float leituras[5] = {0};
int index = 0;
void setup(){  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
    Serial.println("\nTemperatura | Pressão | Altura | Altura Filtrada | Pressão Nivel do Mar | Altitude Real.");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (int i = 0; i < 100; i++) 
    alt += bmp.readAltitude();
  alt = alt/100 ;
}

float filteredAltitude(){
  if (index == 5) {
  index = 0;
  }
  leituras[index] = bmp.readAltitude();
  index++;
  float num = (leituras[0]+leituras[1]+leituras[2]+leituras[3]+leituras[4])/5;
  return num;
}
void loop(){
 
  Serial.print(bmp.readTemperature());
    Serial.print("\t");
  Serial.print(bmp.readPressure());
    Serial.print("\t");
  Serial.print(leituras[index-1] - alt);
  Serial.print("\t");
  Serial.print(filteredAltitude() - alt);
    Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
    Serial.print("\n");
  

}