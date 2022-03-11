#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define chipSelect 53
#define led 13
#define pmt 20 // intervalo de valores usados na para media movel
#define nf (3)  // Numero de filtros 
#define ncp 4  // intervalo de comparação para queda e suibida


Adafruit_BMP085 bmp;
int numero_do_SD; 
int qnt_zero;
int Caindo;
int acendeu;
float z, ref_chao;
float vetor[nf][pmt]; // movimentaçãop dos filtros de sinal de alrura 
float sinal[nf+1];   // irá conter todos sinais relacionado a altura  
float sinalzin[ncp]; // contem os dados usados para comparar a altura 
unsigned long Tempo_Atual = 0;
String Dados_string = ""; // irá conter os dados dos sinais em string
String var_queda; 
String nome_SD;
String txt_SD;
String complemento_SD;
String Projeto_name;
String zeros;
bool Trava1 = true;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  digitalWrite(led, LOW);

  if (!bmp.begin()){Serial.println("Could not find a valid BMP085 sensor, check wiring!");}
  if (!SD.begin(chipSelect)){Serial.println("Card failed, or not present");}
  call_chao(); // calcula a media dos dados para referenciar a altura
  
  // laço para determinar o nome do SD
  numero_do_SD = 0;
  nome_SD = "Kob_";
  txt_SD = ".txt";
  do{
    zeros = "";
    Projeto_name = "";
    complemento_SD = String(numero_do_SD);
    qnt_zero = 8 - nome_SD.length() - complemento_SD.length();
    for (int i = 0; i < qnt_zero; i++) {
      zeros += "0";  
    }
     Projeto_name = nome_SD + zeros + complemento_SD + txt_SD;
     numero_do_SD ++;
     Serial.println(Projeto_name);
  }while(SD.exists(Projeto_name));
  
  
   File dataFile = SD.open(Projeto_name, FILE_WRITE);
   dataFile.print("Alt(m)\t");
   dataFile.print("detecção de queda\t");
   dataFile.print("constante que referencia o chão = ");
   dataFile.println(String(ref_chao));
   dataFile.close();
   
  Serial.print("Temp (*C)\t");
  Serial.print("Pres (Pa)\t");
  Serial.print("Alt (m)\t");
  Serial.println(); 


}
//----------------------------------------------------------------------------------
void loop() {

  // Serial.print(bmp.readTemperature());
  // Serial.print("\t");
  // Serial.print(bmp.readPressure());
  // Serial.print("\t");

  
  sinal[0] = bmp.readAltitude() - ref_chao;
  Filtros();
  Detec_queda();
  Led_para_queda();
  Salvar();

  Serial.print(Dados_string);
  Serial.print("\t");
  Serial.println();
}

//-----------------------------------------------------------------------------
void call_chao() {
  z = 0;
  for (int x = 0; x < 100; x++) {
    z = bmp.readAltitude() + z;
  }
  ref_chao = z / 100; 
}
//----------------------------------------------------------------------------------
void Filtros() {
  for (int y = 0; y < nf; y++) {
    for (int x = pmt - 1; x > 0; x--) {
      vetor[y][x] = vetor[y][x - 1];
    }
    vetor[y][0] = sinal[y];
    float k = 0;
    for (int x = 0; x < pmt; x++) {
      k = vetor[y][x] + k;
    }
    sinal[y + 1] = k / pmt;
  }
}

//----------------------------------------------------------------------------------
void Detec_queda() {
  for (int x = ncp - 1; x > 0; x--) {
      sinalzin[x] = sinalzin[x - 1];
    }
  sinalzin[0] = sinal[nf];
  if ( sinalzin[0] > sinalzin[ncp - 1]){
    var_queda = "1";
  }else{
    var_queda = "0";
  }
    }
//----------------------------------------------------------------------------------
void Led_para_queda() {
   if (var_queda == "0"){
    if(Trava1 == true){
      digitalWrite(led, HIGH);
      acendeu = 1;
      Tempo_Atual = millis();
      Trava1 == false;
    }
  }
  if(Trava1 == false){
    if ((Tempo_Atual - millis()) >= 5000) {
      digitalWrite(led, LOW);
      acendeu = 0;
    }
  }

}

//----------------------------------------------------------------------------------
void Salvar(){
  Dados_string = "";
  for (int y = 0; y < nf+1; y++) {
    Dados_string += String(sinal[y]);
    Dados_string += "\t";
  }
  Dados_string += var_queda;
  File dataFile = SD.open(Projeto_name, FILE_WRITE);
  if(dataFile){
     dataFile.println(Dados_string);
     dataFile.close();
  }else {
    Serial.println("Erro ao abrir o SD");
  } 
}

//----------------------------------------------------------------------------------
