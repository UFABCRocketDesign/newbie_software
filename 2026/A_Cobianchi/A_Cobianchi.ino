#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

// Guarda a altitude inicial (ponto zero)
float altitudeReferencia; 
  
void setup() {
  Serial.begin(115200);
  
  // Verifica se o sensor tá conectado
  if (!bmp.begin()) {
    Serial.println("Erro: Sensor BMP085 nao encontrado!");
    while (1) {} 
  }

  // Salva a altitude do momento exato em que a placa liga como referência
  altitudeReferencia = bmp.readAltitude();

  // Imprime o cabeçalho
  Serial.println("Temp(*C)\tPressao(Pa)\tAlt_Absoluta(m)\tAlt_Relativa(m)");
}
  
void loop() {
  // Lê a altitude do momento
  float altitudeAtual = bmp.readAltitude();
  
  // Calcula o quanto subiu ou desceu em relação ao início
  float altitudeRelativa = altitudeAtual - altitudeReferencia;

  // Envia os dados pra tela no formato de tabela
  Serial.print(bmp.readTemperature());
  Serial.print("\t"); 
  
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  
  Serial.print(altitudeAtual);
  Serial.print("\t");

  Serial.println(altitudeRelativa);

  // Pausa rápida pra não bugar o Monitor Serial
  delay(50); 
}