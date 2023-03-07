#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include <SPI.h>
#include <SD.h>
#include <String.h>

const int chipSelect = 53;

#define n 10
#define n1 5


float alt_inicial; 
float soma; 
int i;
float num[n];
float num2[n];
float num3[n];
int apogeu;
int nun = 0;
String nome = "kaua" + nun;
String arq = nome + ".txt";

float filtro(float mediamovel){
  for(i = n-1; i>0; i--) num[i] = num[i-1];
  num[0] = mediamovel;
  float acc = 0;
  for(i=0; i<n; i++) acc += num[i];

  return acc/n;
}

float filtro2(float mediamovel2){
  for(i = n-1; i>0; i--) num2[i] = num2[i-1];
  num2[0] = mediamovel2;
  float acc2 = 0;
  for(i=0; i<n; i++) acc2 += num2[i];

  return acc2/n;
}

void setup (){
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.print(F("Lendo cartao SD"));
  delay(100);
    if (!SD.begin(chipSelect)) {
      Serial.println(F("Leitura Falhou"));
      while (1);
    }



  while (SD.exists(arq)) {
    nun++;
    nome = "kaua" + nun;
    arq = nome + ".txt";
     while (arq.length() < 8) {
  nome = "0" + nome;
    }
  }

  Serial.println(arq);
  Serial.println(F("Cartao SD Inicializado!"));

  String dataString = "";
        dataString += ("Temperatura (°C) \t");
        dataString += ("Pressão (Pa) \t");
        dataString += ("Altitude (m) \t");
        dataString += ("Altitude com filtro (m) \t");
        dataString += ("Altitude com filtro 2 (m) \t");
        dataString += ("Detecção de Apogeu ");

      File dataFile = SD.open(arq , FILE_WRITE);
       
      Serial.println(dataString);

        if (dataFile) {
          dataFile.println(dataString);
          dataFile.close();
        }
  


  for(i=0; i<5; i++){
    soma += bmp.readAltitude();
  } 
 alt_inicial = soma/5;

}


void loop (){

  float altura = bmp.readAltitude() - alt_inicial;
  float altura_filtrada2 = filtro(filtro2(altura));

  for(i = n1-1; i>0; i--)num3[i] = num3[i-1];
  num3[0] = altura_filtrada2;

  if(num3[0]<num3[1] && num3[1]<num3[2] && num3[2]<num3[3] && num3[3]<num3[4]){
  apogeu = 1;
  }else{
    apogeu = 0;
  }

  
  String dataString = "";
          dataString += String(bmp.readTemperature());
          dataString += "\t";
          dataString += String(bmp.readSealevelPressure());
          dataString += "\t";
          dataString += String(altura);
          dataString += "\t";
          dataString += String(filtro(altura));
          dataString += "\t";
          dataString += String(altura_filtrada2);
          dataString += "\t";
          dataString += String(apogeu);
           
      File dataFile = SD.open(arq , FILE_WRITE);
       
      Serial.println(dataString);

        if (dataFile) {
          dataFile.println(dataString);
          dataFile.close();
        }
          
        else {
          Serial.println(F("ERRO"));
       }

       
  delay(10);
}

