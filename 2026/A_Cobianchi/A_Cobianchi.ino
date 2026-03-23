#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  // --- PARTE 1: Configuração do Sensor ---
  Serial.begin(115200); // Inicia a comunicação com o PC
  
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {} // Se o sensor não for achado, o código trava aqui
  }

  // --- PARTE 2: Configuração do LED ---
  pinMode(LED_BUILTIN, OUTPUT); // Avisa que o pino do LED vai enviar energia (saída)
}
  
void loop() {
  // 1. Liga o LED para indicar que está fazendo a leitura
  digitalWrite(LED_BUILTIN, HIGH);  

  // 2. Faz as leituras do sensor e joga na tela (Monitor Serial)
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
  
  Serial.println(); // Pula uma linha no monitor serial pra ficar fácil de ler

  // 3. Mantém o LED aceso e espera 500 milissegundos (meio segundo)
  delay(500);

  // 4. Apaga o LED e espera mais 500 milissegundos antes de recomeçar
  digitalWrite(LED_BUILTIN, LOW);  
  delay(100); 
}