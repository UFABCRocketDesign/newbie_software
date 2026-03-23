#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  
  if (!bmp.begin()) {
    Serial.println("Erro: Sensor BMP085 nao encontrado!");
    while (1) {} 
  }

  pinMode(LED_BUILTIN, OUTPUT); 

  // --- CABEÇALHO ---
  Serial.println("Temp (*C)\tPressao (Pa)\tAlt (m)\t\tP. Mar (Pa)\tAlt Real (m)");
}
  
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  

  Serial.print(bmp.readTemperature());
  Serial.print("\t\t"); 
  
  Serial.print(bmp.readPressure());
  Serial.print("\t\t");
  
  Serial.print(bmp.readAltitude());
  Serial.print("\t\t");

  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t\t");

  Serial.println(bmp.readAltitude(101500)); 

  delay(500);

  digitalWrite(LED_BUILTIN, LOW);  
  delay(500); 
}