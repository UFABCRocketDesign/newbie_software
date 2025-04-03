#include <Adafruit_BMP085.h>  //Sensor BMP

#define tamanho 10  //Usado pro filtro de dados do BMP

#define IGN_1 36    /*act1*/
#define IGN_2 61	/*act2*/

Adafruit_BMP085 bmp;  //Sensor BMP

#include <SPI.h>  //Usado no DataLogger
#include <SD.h>   //Usado no DataLogger

//Declarando variáveis e arrays pro BMP
float tara = 0;
float vetor[tamanho];
float vetor2[tamanho];
int guia = 0;
int guia2 = 0;
int detectorqueda = 0;
float alturapassada = 0;
float altura_filtrada = 0;
float altura_filtrada2 = 0;

//Declarações pro SD DataLogger
const int chipSelect = 53;
int lognumber = 0;
String nomearquivo;
String nomelog = "ZAMP";
String zerospacetext;
int zerospacelength;

//Declarações pro Paraquedas
int paraquedas1armado = 0;
int paraquedas2armado = 0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillisPRQ1 = 0;  // will store last time LED was updated
unsigned long previousMillisPRQ2 = 0;

// constants won't change:
const long intervalPRQ1 = 5000;  // interval at which to blink (milliseconds)
const long intervalPRQ2 = 5000;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() {
  //COMEÇO DO SETUP DATALOGGER
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial)
    ;

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true)
      ;
  }

  Serial.println("initialization done.");

  do {
    zerospacelength = 8 - nomelog.length() - String(lognumber).length();
    for (int i = 1; i <= zerospacelength; ++i) {
      zerospacetext += "0";
    }
    nomearquivo = nomelog + zerospacetext + String(lognumber) + ".txt";
    lognumber += 1;
    zerospacetext = "";
  } while (SD.exists(nomearquivo));

  Serial.println(nomearquivo);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(nomearquivo, FILE_WRITE);

  //FIM DO SETUP DATALOGGER

  //COMEÇO DO SETUP BMP
  //Inicialização do sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  //

  //Reset de altura
  for (int i = 1; i <= 10; ++i) {
    tara += bmp.readAltitude();
  }
  tara /= 10;
  //

  Serial.print("alturapassada\t");
  Serial.print("Altura Sem Filtro\t");
  Serial.print("Altura Filtrada\t");
  Serial.print("Altura Filtrada 2\t");
  Serial.print("Detector de Queda\t");
  Serial.print("Temperatura\t");
  Serial.print("Pressão\t");
  Serial.print("paraquedasarmado\t");
  Serial.println();
  //FIM DO SETUP BMP

  if (dataFile) {
    dataFile.print("alturapassada\t");
    dataFile.print("Altura Sem Filtro\t");
    dataFile.print("Altura Filtrada\t");
    dataFile.print("Altura Filtrada 2\t");
    dataFile.print("Detector de Queda\t");
    dataFile.print("Temperatura\t");
    dataFile.print("Pressão\t");
    dataFile.print("paraquedas1armado\t");
    dataFile.print("paraquedas2armado\t");
    dataFile.println();
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("Erro ao abrir" + nomearquivo);
  }

  pinMode(IGN_1, OUTPUT);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void loop() {
  unsigned long currentMillis = millis();
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(nomearquivo, FILE_WRITE);

  //COMEÇO DA SEÇÃO DO SENSOR BMP
  //FILTROS
  vetor[guia] = bmp.readAltitude() - tara;  //setup do filtro 1
  if (guia < tamanho - 1) {
    guia += 1;
  } else {
    guia = 0;
  }

  altura_filtrada = 0;                    //reset da altura pra usar no filtro 1
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 1
    altura_filtrada += vetor[i];
  }

  altura_filtrada /= tamanho;  //output do filtro 1

  vetor2[guia2] = altura_filtrada;  //setup do filtro 2
  if (guia2 < tamanho - 1) {
    guia2 += 1;
  } else {
    guia2 = 0;
  }

  altura_filtrada2 = 0;                   //reset da altura pra usar no filtro 2
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 2
    altura_filtrada2 += vetor2[i];
  }
  altura_filtrada2 /= tamanho;  //output do filtro 2

  //DETECTOR DE QUEDA
  if (altura_filtrada2 < alturapassada) {  //Comparação da altura atual pós-filtros com a altura anterior ("alturapassada")
    detectorqueda += 1;
  } else {
    detectorqueda = 0;
  }
  alturapassada = altura_filtrada2;  //Armazenamento da altura atual para usar como "alturapassada" no próximo loop

  // Armazenamento dos valores na dataString
  String dataString = "";
  dataString += String(alturapassada) + "\t";
  dataString += String(vetor[guia]) + "\t";
  dataString += String(altura_filtrada) + "\t";
  dataString += String(altura_filtrada2) + "\t";
  dataString += String(detectorqueda) + "\t";
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";
  dataString += String(paraquedas1armado) + "\t";
  dataString += String(paraquedas2armado) + "\t";

  //Print da dataString
  Serial.println(dataString);

  //FIM DA SEÇÃO DO SENSOR BMP

  //----------------------------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //COMEÇO DA SEÇÃO DO PARAQUEDAS

// Paraquedas 1
  if ((detectorqueda >= 10) && (paraquedas1armado == 0)) {
    digitalWrite(IGN_1, HIGH);
    previousMillisPRQ1 = currentMillis;
    paraquedas1armado = 1;
    Serial.println("Paraquedas 1 Ativado");
  }

  if ((currentMillis - previousMillisPRQ1 >= intervalPRQ1) && (paraquedas1armado == 1)) {
    digitalWrite(IGN_1, LOW);
    paraquedas1armado = 2;
    Serial.println("Paraquedas 1 Desativado");

  }

// Paraquedas 2
  if ((detectorqueda >= 10) && (paraquedas2armado == 0)) {
    previousMillisPRQ2 = currentMillis;
    paraquedas2armado = 3;
    Serial.println("Paraquedas 2 Armado");
  }

  if ((currentMillis - previousMillisPRQ2 >= intervalPRQ2) && (paraquedas2armado == 3)) {
    digitalWrite(IGN_2, HIGH);
    paraquedas2armado = 1;
    Serial.println("Paraquedas 2 Ativado");

  }

  if ((currentMillis - previousMillisPRQ2 >= intervalPRQ2) && (paraquedas2armado == 1)) {
    digitalWrite(IGN_2, LOW);
    paraquedas2armado = 2;
    Serial.println("Paraquedas 2 Desativado");

  }
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //FIM DA SEÇÃO DO PARAQUEDAS

  //----------------------------------------------------------------

  //COMEÇO DA SEÇÃO DO DATALOGGER

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("Erro ao abrir" + nomearquivo);
  }
  //FIM DA SEÇÃO DO DATALOGGER
}
