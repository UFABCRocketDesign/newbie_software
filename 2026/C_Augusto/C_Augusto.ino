#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(9600);
   // inicializa a funcao do pino do led 13
  pinMode(LED_BUILTIN, OUTPUT);
  
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
}
  
void loop() {
  
  digitalWrite(LED_BUILTIN, HIGH);  // muda o pino pra alta tensao
  delay(500);                      // segura 0.5 segundos
  digitalWrite(LED_BUILTIN, LOW);   // muda o pino pra baixa tensao
  delay(500);                      // segura 0.5 segundos  

    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");
    
    Serial.println();
    delay(500);
}