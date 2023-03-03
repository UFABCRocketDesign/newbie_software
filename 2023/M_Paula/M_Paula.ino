//Importando Bibliotecas
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include <SPI.h>
#include <SD.h>


//Declaração de variáveis
#define n_media 5
#define num 10
float alt_inicial;
float values_1[num];
float values_2[num];
float values_3[num];
int accc = 0;
const int chipSelect = 53;



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
  
    if(values_3[0] < values_3[1]){
      accc ++;
    }else{
      accc = 0;
    }
  if(accc >= 10){
    return 1;
  }else{
    return 0;
  }
}



void setup() {
  //Declaração de variáveis
  float soma = 0;
  String dataString = "";
  
  //
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
  dataString += String("Temperatura (*C) ");
  dataString += String("Altura com ruido (meters) ");
  dataString += String("Altura sem ruido (meters) ");
  dataString += String("Pressão (Pa)");
  dataString += String("Valor do accc");
  dataString += String("Situação");
  
  //print dos rótulos das medições
  Serial.print(dataString);
  Serial.print("\n");


  //inicializando a leitura do SD
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print("Initializing SD card...");
  

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  File dataFile = SD.open("marina.txt", FILE_WRITE);

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
 
  
  //impressão dos dados
  Serial.print(temperatura);
  Serial.print("\t");
  Serial.print(altura_com_ruido);
  Serial.print("\t");
  Serial.print(altura_sem_ruido_1);
  Serial.print("\t");
  Serial.print(altura_sem_ruido_2);
  Serial.print("\t");
  Serial.print(pressao);
  Serial.print("\t");
  Serial.print(accc);
  Serial.print("\t");
  Serial.print(situacao);
  Serial.println();

  File dataFile = SD.open("marina.txt", FILE_WRITE);

  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(temperatura);
    dataFile.print("\t");
    dataFile.print(altura_com_ruido);
    dataFile.print("\t");
    dataFile.print(altura_sem_ruido_1);
    dataFile.print("\t");
    dataFile.print(altura_sem_ruido_2);
    dataFile.print("\t");
    dataFile.print(pressao);
    dataFile.print("\t");
    dataFile.print(accc);
    dataFile.print("\t");
    dataFile.print(situacao);
    dataFile.println();
    dataFile.close();
  
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

}