#include <SD.h>

const int chipSelect = 53;

float pressao = 0.0;
float tensao = 0.0;

String zeros;
String filename;
String nome = "LUCAS";
int incremento = 0;
int tamanho = 0;

void setup() {
  Serial.begin(115200);
  pinMode(A0, OUTPUT);

  Serial.print("Inicializando cartão SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Falha na inicialização do cartão SD!");
    while (1)
      ;
  }
  Serial.println("Cartão SD inicializado com sucesso.");

  do {
    tamanho = nome.length() + String(incremento).length();
    zeros = "";
    for (int i = tamanho; i < 8; i++) {
      zeros += "0";
    }
    filename = nome + zeros + String(incremento) + ".txt";
    incremento++;
  } while (SD.exists(filename));

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Transdutor\tTensão\tPressão");
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo");
  }
}

float adcTensao(int adc) {
  tensao = (5.0/1023.0) * adc;
  return tensao;
}

float tensaoPressao(float tensao) {
  pressao = (125 * tensao) - 62.5;
  return pressao;
}

void loop() {
 
  int transdutor = analogRead(A0);
  tensao = adcTensao(transdutor);
  pressao = tensaoPressao(tensao);

  Serial.print(transdutor);
  Serial.print("\t");
  Serial.print(tensao);
  Serial.print("\t");
  Serial.print(pressao);
  Serial.println();

  String dataString = String(transdutor) + "\t" + String(tensao) + "\t" + String(pressao);

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita.");
  }
}