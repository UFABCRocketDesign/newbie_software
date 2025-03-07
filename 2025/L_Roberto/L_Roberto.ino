#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float alt = 0;
float leituras[10] = {0};
int index = 0;
float filteredAltitude(const float);

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

void loop(){
 float altura_atual = bmp.readAltitude() - alt;

  Serial.print(bmp.readTemperature());
    Serial.print("\t");

  Serial.print(bmp.readPressure());
    Serial.print("\t");

  Serial.print(altura_atual);
  Serial.print("\t");

  Serial.print(filteredAltitude(altura_atual));
    Serial.print("\t");

  Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");

  Serial.print(bmp.readAltitude(101500));
    Serial.print("\n");
}


float filteredAltitude(const float altura_atual){
  if (index == 10) {
  index = 0;
  }
  leituras[index] = altura_atual;
  index++;
  float num = 0;
  for (int i = 0; i < index; i++) {
  num += leituras[i];
  }
  return num/index;
}
