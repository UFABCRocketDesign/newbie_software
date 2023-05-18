//Importando Bibliotecas
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include <SPI.h>
#include <SD.h>


//Declaração de variáveis
#define n_media 5
#define num 10
#define name_base "marina"
float alt_inicial;
float values_1[num];
float values_2[num];
float values_3[num];
int accc = 0;
const int chipSelect = 53;
String file_name;



//filtragem da altura com ruido
float media_movel_1(float sinal_com_ruido) {
  float acc = 0;
  for (int i = num - 1; i > 0; i--) {
    values_1[i] = values_1[i - 1];
  }
  values_1[0] = sinal_com_ruido;
  for (int i = 0; i < num; i++) {
    acc += values_1[i];
  }
  return acc / num;
}



//filtragem de altura sem ruido
float media_movel_2(float sinal_sem_ruido) {
  float acc = 0;
  for (int i = num - 1; i > 0; i--) {
    values_2[i] = values_2[i - 1];
  }
  values_2[0] = sinal_sem_ruido;
  for (int i = 0; i < num; i++) {
    acc += values_2[i];
  }
  return acc / num;
}



//Detecção de queda
int queda(float sinal_sem_ruido_2) {
  for (int i = num - 1; i > 0; i--) {
    values_3[i] = values_3[i - 1];
  }
  values_3[0] = sinal_sem_ruido_2;

  if (values_3[0] < values_3[1]) {
    accc++;
  } else {
    accc = 0;
  }
  if (accc >= 10) {
    return 1;
  } else {
    return 0;
  }
}



void setup() {
  //Declaração de variáveis
  float soma = 0;
  String dataString = "";

  //Abrindo serial
  Serial.begin(115200);
  pinMode(13, OUTPUT);

  //Inicializando o sensor de barômetro
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  //cálculo da altitude inicial
  for (int i = 0; i < n_media; ++i) {
    soma = soma + bmp.readAltitude();
  }
  alt_inicial = soma / 5;

  //Salvando os rótulos das medicoes em uma variável
  dataString += "Temperatura (*C) ";
  dataString += "Altura com ruido (meters) ";
  dataString += "Altura sem ruido (meters) ";
  dataString += "Pressão (Pa)";
  dataString += "Valor do accc";
  dataString += "Situação";

  //print dos rótulos das medições
  Serial.println(dataString);


  //inicializando a leitura do SD
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");


  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");
  int i = 0;
  int qtd_zeros = 0;
  String p;
  do{
    p = String(i);
    file_name = name_base;
    if(i<10){
      qtd_zeros = 8 - String(name_base).length() - p.length();
    for(int j = 0; j < qtd_zeros; j++){
      file_name += "0";
      }
    }
    file_name += String(i);
    file_name += ".txt";
    i++;
  }while(SD.exists(file_name));
    
  Serial.println(file_name);

  File dataFile = SD.open(file_name, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}



void loop() {
  //leitura dos dados
  float altura_com_ruido = bmp.readAltitude() - alt_inicial;
  float altura_sem_ruido_1 = media_movel_1(altura_com_ruido);
  float altura_sem_ruido_2 = media_movel_2(altura_sem_ruido_1);
  float temperatura = bmp.readTemperature();
  float pressao = bmp.readPressure();
  int situacao = queda(altura_sem_ruido_2);

  //Salvando os dados espaçados em uma variável
  String dataString = "";
  dataString += String(temperatura);
  dataString += "\t";
  dataString += String(altura_com_ruido);
  dataString += "\t";
  dataString += String(altura_sem_ruido_1);
  dataString += "\t";
  dataString += String(altura_sem_ruido_2);
  dataString += "\t";
  dataString += String(pressao);
  dataString += "\t";
  dataString += String(accc);
  dataString += "\t";
  dataString += String(situacao);


  //impressão dos dados
  Serial.println(dataString);


  File dataFile = SD.open(file_name, FILE_WRITE);


  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(dataString);
    dataFile.println();
    dataFile.close();

  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}