#include <Adafruit_BMP085.h>
#include <SD.h>
Adafruit_BMP085 bmp;

const int numLeituras = 10;
const int chipSelect = 53;
float alt = 0;

float total = 0;
float total2 = 0;

float leituras[numLeituras];
float leituras2[numLeituras];

int indiceAtual = 0;
int indiceAtual2 = 0;
int queda;

String zeros;
String filename;
String nome = "LUCAS";
int incremento = 0;
int tamanho = 0;

float altitudeAnterior = 0;

void setup() {
  Serial.begin(115200);

  Serial.print("Inicializando cartão SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Falha na inicialização do cartão SD!");
    while (1)
      ;
  }
  Serial.println("Cartão SD inicializado com sucesso.");

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  Serial.println("Temperatura\tPressão\tAltitude Filtrada\tAltitude Raw\tPressão Mar\tPressão Local (hPa)\tQueda");

  for (int i = 0; i < 10; i++) {
    alt += bmp.readAltitude();
  }
  alt = alt / 10;

  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = 0;
    leituras2[i] = 0;
  }

  do {
    tamanho = nome.length() + String(incremento).length();
    zeros = "";
    for (int i = tamanho; i < 8; i++) {
      zeros += "0";
    }
    filename = nome + zeros + String(incremento) + ".txt";
    incremento++;
  } while (SD.exists(filename));

  Serial.println(filename);

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Temperatura\tPressão\tAltitudeFiltrada\tAltitudeRaw\tPressãoMar\tPressãoLocal(hPa)\tQueda");
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo");
  }
}

void loop() {
  float altitudeReal = bmp.readAltitude() - alt;

  float temperatura = bmp.readTemperature();
  float pressao = bmp.readPressure();
  float pressaoNivelMar = bmp.readSealevelPressure();
  float altitude = bmp.readAltitude(101500);

  total = total - leituras[indiceAtual];
  leituras[indiceAtual] = (altitudeReal);
  total = total + leituras[indiceAtual];
  indiceAtual = (indiceAtual + 1) % numLeituras;
  float media = total / numLeituras;

  total2 = total2 - leituras2[indiceAtual2];
  leituras2[indiceAtual2] = (media);
  total2 = total2 + leituras2[indiceAtual2];
  indiceAtual2 = (indiceAtual2 + 1) % numLeituras;
  float mediaNova = total2 / numLeituras;

  String dataString = "";
  dataString += String(temperatura) + "\t";
  dataString += String(pressao) + "\t";
  dataString += String(media) + "\t";
  dataString += String(altitudeReal) + "\t";
  dataString += String(pressaoNivelMar) + "\t";
  dataString += String(altitude) + "\t";
  dataString += String(queda);
  
  Serial.println(dataString);

  if (mediaNova < altitudeAnterior) {
    Serial.print(1);
    queda = 1;
  } else {
    Serial.print(0);
    queda = 0;
  }
  altitudeAnterior = mediaNova;

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita.");
  }
}
