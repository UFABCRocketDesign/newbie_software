#include <Adafruit_BMP085.h>  //Sensor BMP

#define tamanho 10  //Usado pro filtro de dados do BMP

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
float altitude_filtrada = 0;
float altitude_filtrada2 = 0;

//Declarações pro SD DataLogger
const int chipSelect = 53;

/////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //COMEÇO DO SETUP DATALOGGER
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
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
  //FIM DO SETUP DATALOGGER

  //COMEÇO DO SETUP BMP
  //Inicialização do sensor
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  //

  //Reset de altitude
  for (int i = 1; i <= 10; ++i) {
    tara += bmp.readAltitude();
  }
  tara /= 10;
  //

  Serial.print("alturapassada\t");
  Serial.print("Altitude Sem Filtro\t");
  Serial.print("Altitude Filtrada\t");
  Serial.print("Altitude Filtrada 2\t");
  Serial.print("Detector de Queda\t");
  Serial.print("Temperatura\t");
  Serial.print("Pressão\t");
  Serial.println();
  //FIM DO SETUP BMP
}
/////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  //COMEÇO DA SEÇÃO DO SENSOR BMP
  //FILTROS
  vetor[guia] = bmp.readAltitude() - tara;  //setup do filtro 1
  if (guia < tamanho - 1) {
    guia += 1;
  } else {
    guia = 0;
  }

  altitude_filtrada = 0;                  //reset da altitude pra usar no filtro 1
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 1
    altitude_filtrada += vetor[i];
  }

  altitude_filtrada /= tamanho;  //output do filtro 1

  vetor2[guia2] = altitude_filtrada;  //setup do filtro 2
  if (guia2 < tamanho - 1) {
    guia2 += 1;
  } else {
    guia2 = 0;
  }

  altitude_filtrada2 = 0;                 //reset da altitude pra usar no filtro 2
  for (int i = 0; i < tamanho; i += 1) {  //cálculo do filtro 2
    altitude_filtrada2 += vetor2[i];
  }
  altitude_filtrada2 /= tamanho;  //output do filtro 2

  //DETECTOR DE QUEDA
  if (altitude_filtrada2 < alturapassada) {  //Comparação da altitude atual pós-filtros com a altitude anterior ("alturapassada")
    detectorqueda = 1;
  } else {
    detectorqueda = 0;
  }
  alturapassada = altitude_filtrada2;  //Armazenamento da altitude atual para usar como "alturapassada" no próximo loop

  // Armazenamento dos valores na dataString
  String dataString = "";
  dataString += String(alturapassada) + "\t";
  dataString += String(vetor[guia]) + "\t";
  dataString += String(altitude_filtrada) + "\t";
  dataString += String(altitude_filtrada2) + "\t";
  dataString += String(detectorqueda) + "\t";
  dataString += String(bmp.readTemperature()) + "\t";
  dataString += String(bmp.readPressure()) + "\t";

  //Print da dataString
  Serial.println(dataString);
  
  //FIM DA SEÇÃO DO SENSOR BMP

  //----------------------------------------------------------------------------------------------------

  //COMEÇO DA SEÇÃO DO DATALOGGER

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  //FIM DA SEÇÃO DO DATALOGGER
}
