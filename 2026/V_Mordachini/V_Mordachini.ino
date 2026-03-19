// Victor :)
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

//variaveis de altitude
float altitudeInicial;
const int medicoes = 30; // numero de leituras para calibragem

//config filtro EMA
//float alturaFiltradaEMA = 0; // valor filtrado
//const float alpha = 0.1;   // fator de suavização

//config filtro SMA 1 
const int tamanhoJanela = 10; // quantidade de leituras para media
float leituras[tamanhoJanela];
int indice = 0; // posicão atual no vetor leituras
float somaSMA = 0;
float alturaFiltradaSMA = 0;

//config filtro SMA 2 (cascata - refinamento)
const int tamanhoJanela2 = 10;
float leituras2[tamanhoJanela2];
int indice2 = 0;
float somaSMA2 = 0;
float alturaFinal = 0;


void setup() {
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  //LOGICA DE CALIBRACAO
  float soma = 0;
  for (int i = 0; i < medicoes; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma / medicoes; // altura 0
  
  // inicializa o filtro EMA com o valor zero
  //alturaFiltradaEMA = 0;

  // inicializa os vetores dos filtros SMA com zeros
  for (int i = 0; i < tamanhoJanela; i++) leituras[i] = 0;
  for (int i = 0; i < tamanhoJanela2; i++) leituras2[i] = 0;


  //CABECALHO
  Serial.println("Temp(C)\tPressao(Pa)\tAlturaBruta(m)\tAlturaFiltradaSMA(m)\tAlturaFinal(m)");
}

void loop() {
  // leitura bruta
  float altitudeAtual = bmp.readAltitude();
  float alturaBruta = altitudeAtual - altitudeInicial;

  // filtro EMA (media movel exponencial)
  // alturaFiltrada recebe 10% da leitura nova e mantém 90% da anterior
  //alturaFiltradaEMA = (alpha * alturaBruta) + (1.0 - alpha) * alturaFiltradaEMA;

  //filtro SMA 1 (media movel simples) 
  somaSMA -= leituras[indice];
  leituras[indice] = alturaBruta;
  somaSMA += leituras[indice]; 
  indice++; 
  
  if (indice >= tamanhoJanela) { // 5 - se chegar no fim do array, volta ao início
    indice = 0;
  }
  
  alturaFiltradaSMA = somaSMA / tamanhoJanela;

  //segundo filtro SMA (refinamento)
  somaSMA2 -= leituras2[indice2];
  leituras2[indice2] = alturaFiltradaSMA;
  somaSMA2 += leituras2[indice2]; 
  indice2++; 
  
  if (indice2 >= tamanhoJanela2) {
    indice2 = 0;
  }
  
  alturaFinal = somaSMA2 / tamanhoJanela2;



  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(alturaBruta);
  Serial.print("\t");

  Serial.print(alturaFiltradaSMA);
  Serial.print("\t");

  Serial.println(alturaFinal);
}