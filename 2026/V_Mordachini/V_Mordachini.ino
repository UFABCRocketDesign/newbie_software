// Victor :)
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;

// configuracoes SD
const int chipSelect = 53;
String fileName = "";

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

void setup() {
  Serial.begin(115200);

  // Inicializa Sensor BMP085
  if (!bmp.begin()) {
    Serial.println("Checar sensor!");
    while (1) {}
  }

  // Inicializa Cartao SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Checar cartao SD");

  } else {
    // Logica de auto-nomeacao (incremental) do arquivo sd
    int numArquivo = 1;
    String prefixo = "VIC"; // pode ser alterado

    // Procura o primeiro num de arquivo que ainda não existe
    while (true) {
      String numStr = String(numArquivo);
      // Adiciona zeros à esquerda até que o nome tenha EXATAMENTE 8 caracteres
      while ((prefixo.length() + numStr.length()) < 8) {
        numStr = "0" + numStr;
      }

      // Concatena tudo: VIC + 00001 + .txt = VIC00001.txt
      fileName = prefixo + numStr + ".txt"; 
      
      if (!SD.exists(fileName)) {
        break; // Achou um nome livre
      }

      numArquivo++;
    }

    Serial.print("Arquivo criado: ");
    Serial.println(fileName);

    // Cria o aruivo com cabeçalho
    File dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println("Temp(C),Pressao(Pa),AltBruta(m),AltFinal(m),Max(m),Queda");
      dataFile.close();
    }
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
  // leitura dos sensores 
  float altitudeAtual = bmp.readAltitude();
  float alturaBruta = altitudeAtual - altitudeInicial;
  float tempAtual = bmp.readTemperature();
  float pressAtual = bmp.readPressure();

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
    if (alturaFinal < alturaAnterior) {
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

  // Montagem string de dados
  String dataString = "";
  dataString += String(tempAtual);     dataString += "\t";
  dataString += String(pressAtual);    dataString += "\t";
  dataString += String(alturaBruta);   dataString += "\t";
  dataString += String(alturaFinal);   dataString += "\t";
  dataString += String(alturaMaxima);  dataString += "\t";
  dataString += String(quedaDetectada ? 1 : 0);

  // GRAVACAO NO SD
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    // Se falhar o SD, avisamos no serial, mas o loop continua
    Serial.println("Erro ao gravar no SD!"); 
  }

  // ---- SAIDA DE DADOS SERIAL----
  Serial.println(dataString);
}