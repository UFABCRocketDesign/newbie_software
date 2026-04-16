// Victor :) - Rocket Design
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

// ==========================================
// 1. OBJETOS E CONFIGURAÇÕES DE HARDWARE
// ==========================================
Adafruit_BMP085 bmp;

// Pinos do Cartão SD e Ignitores
const int chipSelect = 53;
#define IGN_1 36  // Atuador 1
#define IGN_2 61  // Atuador 2
#define IGN_3 46  // Atuador 3
#define IGN_4 55  // Atuador 4

// ==========================================
// 2. VARIÁVEIS GLOBAIS
// ==========================================
String fileName = ""; // Nome global do arquivo de log (Ex: VIC00001.txt)

// --- Calibração ---
float altitudeInicial;
const int medicoes = 30; 

// --- Filtros SMA (Media Movel Simples) ---
const int tamanhoJanela = 10; 
float leituras[tamanhoJanela];
int indice = 0; 
float somaSMA = 0;
float alturaFiltradaSMA = 0;

const int tamanhoJanela2 = 10;
float leituras2[tamanhoJanela2];
int indice2 = 0;
float somaSMA2 = 0;
float alturaFinal = 0; // Resultado processado

// --- Máquina de Estados e Recuperação ---
enum EstadoVoo {
  SUBINDO,
  ACIONANDO_PARAQUEDAS,
  FINALIZADO
};
EstadoVoo estadoAtual = SUBINDO; // Estado inicial do foguete

unsigned long tempoInicioAcionamento = 0;
const unsigned long tempoEjecao = 10000; // Duração do acionamento: 10 segundos

// --- Lógica de Detecção de Apogeu ---
float alturaMaxima = 0.0;
float alturaAnterior = 0.0;
int contadorQuedaPares = 0;
const int confirmacoesNecessarias = 30; 
const float margemEmergencia = 10.0;    // Disparo forçado se cair 10m do pico


// ==========================================
// 3. SETUP - INICIALIZAÇÃO DO SISTEMA
// ==========================================
void setup() {
  Serial.begin(115200);

  // --- 3.1. Segurança dos Atuadores ---
  // Configura como saída e força o estado LOW para evitar disparos na rampa
  pinMode(IGN_1, OUTPUT); digitalWrite(IGN_1, LOW);
  pinMode(IGN_2, OUTPUT); digitalWrite(IGN_2, LOW);
  pinMode(IGN_3, OUTPUT); digitalWrite(IGN_3, LOW);
  pinMode(IGN_4, OUTPUT); digitalWrite(IGN_4, LOW);

  // --- 3.2. Inicialização do Sensor ---
  if (!bmp.begin()) {
    Serial.println("Erro: Checar sensor BMP085!");
    while (1) {} // Trava o sistema se não achar o sensor
  }

  // --- 3.3. Inicialização do Cartão SD e Nomeação ---
  if (!SD.begin(chipSelect)) {
    Serial.println("Erro: Checar cartao SD!");
  } else {
    // Busca um nome de arquivo inédito preenchendo com zeros à esquerda (Padrão 8.3)
    int numArquivo = 1;
    String prefixo = "VIC"; 
    
    while (true) {
      String numStr = String(numArquivo);
      while ((prefixo.length() + numStr.length()) < 8) {
        numStr = "0" + numStr;
      }
      fileName = prefixo + numStr + ".txt"; 
      if (!SD.exists(fileName)) break; 
      numArquivo++;
    }

    Serial.print("Arquivo criado: ");
    Serial.println(fileName);

    // Grava o cabeçalho no novo arquivo
    File dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println("Tempo(ms)\tTemp(C)\tPress(Pa)\tBruta(m)\tFinal(m)\tMax(m)\tEstado");
      dataFile.close();
    }
  }

  // --- 3.4. Calibração de Altitude Zero ---
  float soma = 0;
  for (int i = 0; i < medicoes; i++) {
    soma += bmp.readAltitude();
  }
  altitudeInicial = soma / medicoes; 

  // --- 3.5. Limpeza de Memória ---
  for (int i = 0; i < tamanhoJanela; i++) leituras[i] = 0;
  for (int i = 0; i < tamanhoJanela2; i++) leituras2[i] = 0;

  Serial.println("Tempo\tTemp(C)\tPressao(Pa)\tBruta(m)\tSMA1(m)\tFinal(m)\tMax(m)\tEstado");
}


// ==========================================
// 4. LOOP - COMPUTADOR DE VOO
// ==========================================
void loop() {
  unsigned long tempoAtualVoo = millis(); // Relógio interno do ciclo atual

  // --- 4.1. Aquisição de Dados ---
  float altitudeAtual = bmp.readAltitude();
  float alturaBruta = altitudeAtual - altitudeInicial;
  float tempAtual = bmp.readTemperature();
  float pressAtual = bmp.readPressure();

  // --- 4.2. Processamento de Sinais (Filtro Duplo) ---
  // Estágio 1
  somaSMA -= leituras[indice];
  leituras[indice] = alturaBruta;
  somaSMA += leituras[indice]; 
  indice++; 
  if (indice >= tamanhoJanela) indice = 0;
  alturaFiltradaSMA = somaSMA / tamanhoJanela;

  // Estágio 2 (Cascata)
  somaSMA2 -= leituras2[indice2];
  leituras2[indice2] = alturaFiltradaSMA;
  somaSMA2 += leituras2[indice2]; 
  indice2++; 
  if (indice2 >= tamanhoJanela2) indice2 = 0;
  alturaFinal = somaSMA2 / tamanhoJanela2;

  // Atualiza recorde de voo
  if (alturaFinal > alturaMaxima) {
    alturaMaxima = alturaFinal;
  }

  // --- 4.3. Máquina de Estados de Recuperação ---
  
  if (estadoAtual == SUBINDO) {
    // A) Sistema Primário: Analisa a tendência de queda contínua
    if (alturaFinal < alturaAnterior) {
      contadorQuedaPares++;
      if (contadorQuedaPares >= confirmacoesNecessarias) {
        estadoAtual = ACIONANDO_PARAQUEDAS;
      }
    } else {
      contadorQuedaPares = 0; // Zera se oscilar para cima
    }

    // B) Sistema de Emergência: Trava de margem absoluta
    if (alturaFinal < (alturaMaxima - margemEmergencia)) {
      estadoAtual = ACIONANDO_PARAQUEDAS;
    }

    // C) Transição: Executa a ejeção (Liga o led)
    if (estadoAtual == ACIONANDO_PARAQUEDAS) {
      digitalWrite(IGN_1, HIGH);
      tempoInicioAcionamento = millis(); // Tira "foto" do relógio
    }
    alturaAnterior = alturaFinal;
  } 
  
  else if (estadoAtual == ACIONANDO_PARAQUEDAS) {
    // Monitora o tempo sem travar o processador
    if (tempoAtualVoo - tempoInicioAcionamento >= tempoEjecao) {
      digitalWrite(IGN_1, LOW); // Corta a corrente após 10s
      estadoAtual = FINALIZADO; 
    }
  }

  // Se estadoAtual == FINALIZADO, o código apenas pula a máquina de estados e continua gravando.

  // --- 4.4. Armazenamento de Dados ---
  String dataString = "";
  dataString += String(tempoAtualVoo); dataString += "\t";
  dataString += String(tempAtual);     dataString += "\t";
  dataString += String(pressAtual);    dataString += "\t";
  dataString += String(alturaBruta);   dataString += "\t";
  dataString += String(alturaFinal);   dataString += "\t";
  dataString += String(alturaMaxima);  dataString += "\t";
  dataString += String(estadoAtual);   

  // Grava no SD
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }

  // Saída de monitoramento (Serial)
  Serial.println(dataString);

}