#include <Adafruit_BMP085.h>
#include <SD.h>
Adafruit_BMP085 bmp;

#define IGN_1 36
#define IGN_2 61
#define IGN_3 46
#define IGN_4 55
#define LEITURAS 10
#define CHIP_SELECT 53
#define NUMERO_QUEDAS 5

float alt = 0;
float total = 0;
float total2 = 0;

float leituras[LEITURAS];
float leituras2[LEITURAS];
float altitudes[NUMERO_QUEDAS];

int indiceAtual = 0;
int indiceAtual2 = 0;
int contadorQueda = 0;
int queda = 0;

String zeros;
String filename;
String nome = "LUCAS";
int incremento = 0;
int tamanho = 0;

float altitudeAnterior = 0;
int altitudeTeto = -3;

int paraquedas_1 = 0;
int paraquedas_2 = 0;
int paraquedas_3 = 0;
int paraquedas_4 = 0;
unsigned long desativacao_p1 = 0;
unsigned long desativacao_p2 = 0;
unsigned long desativacao_p3 = 0;
unsigned long timer_p1;
unsigned long timer_p2;
unsigned long timer_p3;
unsigned long timer_p4;

void setup() {
  Serial.begin(115200);

  Serial.print("Inicializando cartão SD...");
  if (!SD.begin(CHIP_SELECT)) {
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

  for (int i = 0; i < LEITURAS; i++) {
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
    dataFile.println("Temperatura\tPressão\tAltitudeFiltrada\tAltitudeRaw\tPressãoMar\tPressãoLocal(hPa)\tQueda]tParaquedas_1\tParaquedas_2");
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo");
  }

  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
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
  indiceAtual = (indiceAtual + 1) % LEITURAS;
  float media = total / LEITURAS;

  total2 = total2 - leituras2[indiceAtual2];
  leituras2[indiceAtual2] = (media);
  total2 = total2 + leituras2[indiceAtual2];
  indiceAtual2 = (indiceAtual2 + 1) % LEITURAS;
  float mediaNova = total2 / LEITURAS;

  if (mediaNova < altitudeAnterior) {
    contadorQueda++;
  } else {
    contadorQueda = 0;
  }
  altitudeAnterior = mediaNova;

  if (contadorQueda >= 10) {
    queda = 1;
  } else {
    queda = 0;
  }


  /* Paraquedas 1 e 2 */
  if (queda == 1 && paraquedas_1 == 0) {
    paraquedas_1 = 1;
    digitalWrite(IGN_1, HIGH);
    timer_p1 = millis();
  }

  if (paraquedas_1 == 1 && (millis() - timer_p1) >= 2000) {
    paraquedas_1 = 2;
    digitalWrite(IGN_1, LOW);
  }

  if (queda == 1 && paraquedas_2 == 0) {
    paraquedas_2 = 2;
    timer_p2 = millis();
  }

  if (paraquedas_2 == 1 && (millis() - timer_p2) >= 2000) {
    paraquedas_2 = 1;
    digitalWrite(IGN_2, HIGH);
    desativacao_p2 = millis();
  }

  if (paraquedas_2 == 2 && (millis() - desativacao_p2) >= 2000) {
    paraquedas_2 = 3;
    digitalWrite(IGN_2, LOW);
  }

  /* Paraquedas 3 e 4 */
  if (queda == 1 && paraquedas_3 == 0 && mediaNova < altitudeTeto) {
    paraquedas_3 = 1;
    digitalWrite(IGN_3, HIGH);
    timer_p3 = millis();
  }

  if (paraquedas_3 == 1 && (millis() - timer_p3) >= 2000) {
    paraquedas_3 = 2;
    digitalWrite(IGN_3, LOW);
  }

  if (queda == 1 && paraquedas_3 == 0) {
    paraquedas_3 = 2;
    timer_p3 = millis();
  }

  if (paraquedas_3 == 1 && (millis() - timer_p2) >= 2000) {
    paraquedas_3 = 1;
    digitalWrite(IGN_3, HIGH);
    desativacao_p3 = millis();
  }

  if (paraquedas_3 == 2 && (millis() - desativacao_p3) >= 2000) {
    paraquedas_3 = 3;
    digitalWrite(IGN_3, LOW);
  }

  String dataString = "";
  dataString += String(temperatura) + "\t";
  dataString += String(pressao) + "\t";
  dataString += String(media) + "\t";
  dataString += String(mediaNova) + "\t";
  dataString += String(altitudeReal) + "\t";
  dataString += String(pressaoNivelMar) + "\t";
  dataString += String(altitude) + "\t";
  dataString += String(queda) + "\t";
  dataString += String(paraquedas_1) + "\t";
  dataString += String(paraquedas_2) + "\t";
  dataString += String(paraquedas_3);

  Serial.println(dataString);

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita.");
  }
}