#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include <SPI.h>
#include <SD.h>
#include <String.h>

const int chipSelect = 53;

#define n 10
#define n1 5
#define IGN_1 36	/*act1*/
#define IGN_2 61	/*act2*/
#define IGN_3 46	/*act3*/
#define IGN_4 55	/*act4*/


float alt_inicial; 
float soma; 
int i;
float num[n];
float num2[n];
float num3[n];
int apogeu;
int nun = 0;
String nome = "kaua";
String arq = nome + nun + ".txt";
int startTime;
int duration = 10000; 
bool ledState = false;


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
  pinMode(IGN_1, OUTPUT);
  pinMode(13,OUTPUT);
  startTime = millis();
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.println(F("Lendo cartao SD"));
  delay(100);
    if (!SD.begin(chipSelect)) {
      Serial.println(F("Leitura Falhou"));
      while (1);
    }

  do {
      String zeros;
      for(i=String(nun).length()+String(nome).length(); i<8; i++){
      zeros = zeros + "0";
    } 
    arq = nome + zeros + nun + ".txt";
    nun++;
  } while (SD.exists(arq));

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
  float altura_filtrada = filtro(altura);
  float altura_filtrada2 = filtro2(altura_filtrada);

  for(i = n1-1; i>0; i--)num3[i] = num3[i-1];
  num3[0] = altura_filtrada2;

  if(num3[0]<num3[1] && num3[1]<num3[2] && num3[2]<num3[3] && num3[3]<num3[4]){
  apogeu = 1;
  }else{
    apogeu = 0;
  }

  if (apogeu == 1) {
    unsigned long currentTime = millis(); 
    if (currentTime - startTime < duration) {
      if (!ledState) {
        digitalWrite(IGN_1, HIGH); 
        ledState = true;
      }
    } else {
      if (ledState) {
        digitalWrite(IGN_1, LOW); 
        ledState = false;
        startTime = currentTime; 
      }
    }
  }

  
  String dataString = "";
          dataString += String(bmp.readTemperature());
          dataString += "\t";
          dataString += String(bmp.readSealevelPressure());
          dataString += "\t";
          dataString += String(altura);
          dataString += "\t";
          dataString += String(altura_filtrada);
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
  
  Serial.println();

  delay(10);
}

