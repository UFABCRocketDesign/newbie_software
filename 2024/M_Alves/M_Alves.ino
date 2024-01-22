#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float AltInicial = 0;
int numLeiturasInicial = 25;
float somaAltInicial = 0;

// Variáveis para a média simples
const int numLeituras = 15;   // número de leituras para a média
float leituras[numLeituras];  // as leituras do sensor de altitude
int indiceLeitura = 0;        // o índice da leitura atual
float somaLeituras = 0;       // a soma das leituras
float mediaAltitude = 0;      // a média das leituras

// Variáveis para o segundo filtro
float leiturasFiltradas[numLeituras];  // as leituras filtradas
float somaLeiturasFiltradas = 0;       // a soma das leituras filtradas
float mediaAltitudeFiltrada = 0;       // a média das leituras filtradas

//apogeu
float apogeu = 5;
int contador = 0;

void setup() {
  //BME085
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  //Cabeçalho
  Serial.println("Temperature (*C) \t Pressure (Pa) \t Raw Altitude \t First Filter (m) \t Second Filter");

  //Leituras iniciais
  for (int i = 0; i < numLeiturasInicial; i++) {
    somaAltInicial += bmp.readAltitude();
  }

  AltInicial = somaAltInicial / numLeiturasInicial;  //Médias das leituras iniciais

  //Armazena x leituras para realizar a média
  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = bmp.readAltitude() - AltInicial;
    somaLeituras += leituras[i];
    leiturasFiltradas[i] = leituras[i];
    somaLeiturasFiltradas += leiturasFiltradas[i];
  }
}

// the loop function runs over and over again forever
void loop() {

  //BME085
  Serial.print(bmp.readTemperature());
  Serial.print('\t');
  Serial.print(bmp.readPressure());
  Serial.print('\t');

  float rawAltitude = bmp.readAltitude() - AltInicial;
  Serial.print(rawAltitude);  //Altura do sensor sem filtro, rawww
  Serial.print('\t');

  // Primeiro Filtro
  somaLeituras = somaLeituras - leituras[indiceLeitura];
  leituras[indiceLeitura] = rawAltitude;
  somaLeituras = somaLeituras + leituras[indiceLeitura];
  mediaAltitude = somaLeituras / numLeituras;
  Serial.print(mediaAltitude);
  Serial.print('\t');

  // Segundo Filtro
  somaLeiturasFiltradas = somaLeiturasFiltradas - leiturasFiltradas[indiceLeitura];
  leiturasFiltradas[indiceLeitura] = mediaAltitude;  // usa a média do primeiro filtro
  somaLeiturasFiltradas = somaLeiturasFiltradas + leiturasFiltradas[indiceLeitura];
  mediaAltitudeFiltrada = somaLeiturasFiltradas / numLeituras;
  Serial.println(mediaAltitudeFiltrada);

  indiceLeitura++;
  if (indiceLeitura >= numLeituras) {  //se for o último vetor, volta para o início
    indiceLeitura = 0;
  }

  //Apogeu
  if (mediaAltitudeFiltrada >= apogeu) {
    contador++;
    if (contador > 20) {
      Serial.println("APOGEU! ative alguma coisa com isso pfvr");
    }
  }
}