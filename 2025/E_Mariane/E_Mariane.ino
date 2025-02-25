//projeto LED

//void setup() {
//  pinMode(13, OUTPUT); //pino 13 como saída
//}

//void loop() {
//  digitalWrite(13, HIGH); 
//  delay(1500);            
//  digitalWrite(13, LOW);  
//  delay(500);            
//}



#include <Adafruit_BMP085.h> //biblioteca
Adafruit_BMP085 sensor; //representa o sensor, a biblioteca precisa para se comunicar com o sensor, pode ser qualquer nome)
  
void LEDblink() { //a função que pisca o led
    digitalWrite(13, HIGH); 
    delay(1500);            
    digitalWrite(13, LOW);  
    delay(250); 

}
void bmp() { //função que lê o sensor
    Serial.print("Temperature = ");
    Serial.print(sensor.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(sensor.readPressure());
    Serial.println(" Pa");
    
    Serial.print("Altitude = ");
    Serial.print(sensor.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(sensor.readSealevelPressure());
    Serial.println(" Pa");

    Serial.print("Real altitude = ");
    Serial.print(sensor.readAltitude(101500));
    Serial.println(" meters");
    
    Serial.println();
    delay(1000);
}

void setup() {
  pinMode(13, OUTPUT); //pino led como saida
  Serial.begin(115200);
  

  if (!sensor.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}

void loop() { //dps de definir as funções, vai ficar rodando sempre
  LEDblink();
  bmp();
}

