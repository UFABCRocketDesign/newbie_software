#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float altitudeInicial; //definiçao da altura inicial

//inicio de vetor e variavel acumulativa da altura relativa
const int numLeituras = 10; 
float leituras[numLeituras];
int indice = 0;
float total = 0;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  altitudeInicial = bmp.readAltitude(); //definição da altura inicial
  // Preenche o array do filtro com zeros
  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = 0;
  }
  
  Serial.println("*C\tPa\tmeters\tPa\tmeters");
}

void loop() {
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  //definição da altura relativa = 0
  //float altitudeRelativa = bmp.readAltitude() - altitudeInicial; 
  //Serial.print(altitudeRelativa); //print altura relativa

// Subtrai a leitura mais antiga do total
  total = total - leituras[indice];
  // Lê a nova altitude relativa (subtraindo o x0)
  leituras[indice] = bmp.readAltitude() - altitudeInicial;
// Adiciona a nova leitura ao total
  total = total + leituras[indice];
// Avança para a próxima posição do array (volta ao 0 se chegar no limite)
  indice = (indice + 1) % numLeituras;
  // Calcula a média móvel
  float mediaMovel = total / numLeituras;
  // --- ZONA MORTA (ESTABILIZADOR DE ZERO) ---
  // Se a variação for menor que 1 metro (ex: oscilando entre -0.8 e 0.8), trava no 0.
  // Você pode ajustar esse limite (0.8) de acordo com o ruído do seu sensor.
  if (abs(mediaMovel) < 0.8) {
    mediaMovel = 0.0;

  // Imprime o resultado ajustado sem casas decimais (1 algarismo significativo para o zero)
  Serial.print(round(mediaMovel)); 
  Serial.print("\t");
  Serial.println();
}