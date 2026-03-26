// Victor :)
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

// variaveis de altitude
float altitudeInicial;
const int medicoes = 30; // numero de leituras para calibragem

// config filtro SMA 1 
const int tamanhoJanela = 10; // quantidade de leituras para media
float leituras[tamanhoJanela];
int indice = 0; // posicão atual no vetor leituras
float somaSMA = 0;
float alturaFiltradaSMA = 0;

// config filtro SMA 2 (cascata - refinamento)
const int tamanhoJanela2 = 10;
float leituras2[tamanhoJanela2];
int indice2 = 0;
float somaSMA2 = 0;
float alturaFinal = 0;

// variaveis para deteccao de queda
float alturaMaxima = 0.0;
bool quedaDetectada = false; // muda para true ao detectar queda
const float margemEmergencia = 10.0; // margem de seguranca para abrir paraquedas

float alturaAnterior = 0.0;
int contadorQuedaPares = 0;
const int confirmacoesNecessarias = 30;
const float toleranciaPares = 0.02; // tolerancia de 2 centímetros (0.02m) por leitura

void setup() {
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  // LOGICA DE CALIBRACAO
  float soma = 0;
  for (int i = 0; i < medicoes; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma / medicoes; // altura 0

  // inicializa os vetores dos filtros SMA com zeros
  for (int i = 0; i < tamanhoJanela; i++) leituras[i] = 0;
  for (int i = 0; i < tamanhoJanela2; i++) leituras2[i] = 0;

  // CABECALHO
  Serial.println("Temp(C)\tPressao(Pa)\tBruta(m)\tSMA1(m)\tFinal(m)\tMax(m)\tEstado");
}

void loop() {
  // leitura bruta
  float altitudeAtual = bmp.readAltitude();
  float alturaBruta = altitudeAtual - altitudeInicial;

  // filtro SMA 1 (media movel simples) 
  somaSMA -= leituras[indice];
  leituras[indice] = alturaBruta;
  somaSMA += leituras[indice]; 
  indice++; 
  if (indice >= tamanhoJanela) indice = 0;
  alturaFiltradaSMA = somaSMA / tamanhoJanela;

  // segundo filtro SMA (refinamento)
  somaSMA2 -= leituras2[indice2];
  leituras2[indice2] = alturaFiltradaSMA;
  somaSMA2 += leituras2[indice2]; 
  indice2++; 
  if (indice2 >= tamanhoJanela2) indice2 = 0;
  alturaFinal = somaSMA2 / tamanhoJanela2;

  // LOGICA PARA DETECCAO DE QUEDA

  // 1. atualiza record de altura
  if (alturaFinal > alturaMaxima) {
    alturaMaxima = alturaFinal;
  }

  // 2. verifica se o foguete comecou a cair (caso queda nao detectada)
  if (!quedaDetectada) {
    
    // SISTEMA PRIMARIO: Tendência de Queda (Pares)
    if (alturaFinal < alturaAnterior - toleranciaPares) {
      contadorQuedaPares++;

      if (contadorQuedaPares >= confirmacoesNecessarias) {
        quedaDetectada = true;
      }
    } else {
      // zera o contador caso a altura se mantenha ou suba
      contadorQuedaPares = 0; 
    }

    // SISTEMA DE EMERGENCIA: Margem Absoluta
    // se a altura despencar alem da margem, aciona independente dos pares
    if (alturaFinal < (alturaMaxima - margemEmergencia)) {
      quedaDetectada = true;
    }

    // salva a altura de agora para comparar no prox ciclo
    alturaAnterior = alturaFinal;
  }

  // ---- SAIDA DE DADOS ----
  Serial.print(bmp.readTemperature()); // temperatura
  Serial.print("\t");

  Serial.print(bmp.readPressure()); // pressao
  Serial.print("\t");

  Serial.print(alturaBruta);
  Serial.print("\t");

  Serial.print(alturaFiltradaSMA);
  Serial.print("\t");

  Serial.print(alturaFinal);
  Serial.print("\t");

  Serial.print(alturaMaxima);
  Serial.print("\t");

  Serial.println(quedaDetectada ? 1 : 0);
}