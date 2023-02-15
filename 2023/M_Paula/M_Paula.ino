//Importando Bibliotecas
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;



//Declaração de variáveis
#define n_media 5
#define num 10
float alt_inicial;
float values_1[num];
float values_2[num];
float values_3[num];
int accc = 0;



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
  Serial.print(accc);
  if(accc >= 10){
    return 1;
  }else{
    return 0;
  }
}



void setup() {
  float soma = 0;
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
  
  //print dos rótulos das medicoes
  Serial.print("Temperatura (*C) ");
  Serial.print("Altura com ruido (meters) ");
  Serial.print("Altura sem ruido (meters) ");
  Serial.print("Pressão (Pa)");
   Serial.println("Valor do accc");
  Serial.println("Situação");

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

}