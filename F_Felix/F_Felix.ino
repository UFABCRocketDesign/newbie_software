#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/
#define intervalo 10000
#define intervalo2 12000

Adafruit_BMP085 bmp;
float auxiliar = 0;
float media = 0;
float soma = 0;
float mediaMovel = 0;
float segundaMediaMovel = 0;
float matriz[2][10];
String arquivo = "";
int num = 0;
int led = LOW;
int led2 = LOW; 
int led3 = LOW;
int led4 = LOW;       
unsigned long tempoAtual = 0; 
unsigned long desligaLED = 0; 
unsigned long desligaLED2 = 0;     
unsigned long ligaLED2 = 0;
float apogeu = 0;   
boolean detectaApogeu = false;                

const int chipSelect = 53;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(IGN_1, OUTPUT);
    pinMode(IGN_2, OUTPUT);

    Serial.begin(115200);
  
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    
    }

    while (!Serial) {
      ; 
    }


    Serial.print("Initializing SD card...");

    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      while (1);
    }
    Serial.println("card initialized.");

    String dataString = "";
    
    dataString += ("Temp.(*C)\t");

    dataString += ("Pres.(Pa)\t");

    dataString += ("Alt.(m)\t");

    dataString += ("Pres. sealevel(Pa)\t");

    dataString += ("Real alt.(m)\t");

    dataString += ("Foguete\t");
    
    dataString += ("Variação\t");
    
    dataString += ("Média\t");
    
    dataString += ("Média 2\t");
    
    String nome = "f";
    int cont = 0;
    String aux = (String) cont;
    String txt = ".txt";
    bool existente = false;

  while(existente == false){
      
      String aux = (String) cont;
      int tamanho = 8 - (aux.length() + nome.length());
      String zero = "";
      
      for( int i = 0; i < tamanho; i++){
        zero = zero + "0";
      }

      arquivo = nome + zero + aux + txt;
      
      if(SD.exists(arquivo)) {
        cont = cont + 1;
        existente = false;
      }
      else {
        File dataFile = SD.open(arquivo, FILE_WRITE);
        dataFile.println(dataString);
        dataFile.close();
        existente = true;
      }
      
      Serial.println(arquivo);
  }

    float alt = bmp.readAltitude();

    for (int j = 0; j < 100; j++){
      soma += alt;
    }

    media = soma/100.0;
}

// the loop function runs over and over again forever
void loop() { 

    tempoAtual = millis();
    
    float alt = bmp.readAltitude();
    float somaVet = 0;
    float somaVet2 = 0;

    float altRelativa = alt - media;

    for (int i = 0; i < 9; i++){
         matriz[0][i] = matriz[0][i+1];
     }
     
     matriz[0][9] = altRelativa;
  
    
     for (int i = 0; i < 10; i++){
        somaVet += matriz[0][i];
      }

    mediaMovel = somaVet/10.0;

    
      for (int i = 0; i < 9; i ++) {
        matriz [1][i] = matriz [1][i+1];
      }
      matriz[1][9] = mediaMovel;

      
      for (int i = 0; i < 10; i++){
        somaVet2 += matriz[1][i];
      }

    segundaMediaMovel = somaVet2/10.0;

    String dataString = "";
    
    dataString+= String(bmp.readTemperature());
    dataString+= ("\t");

    dataString+= String(bmp.readPressure());
    dataString+= ("\t");

    dataString+= String(bmp.readAltitude(101500));
    dataString+= ("\t");

    dataString += String (bmp.readSealevelPressure());
    dataString += ("\t");

    dataString += String(altRelativa);
    dataString += ("\t");

    if (apogeu < segundaMediaMovel) {                                    
        apogeu = segundaMediaMovel;
    }

    float diferenca = apogeu - segundaMediaMovel;
    
    if(diferenca >= 1){
      dataString += String("caindo\t");
      
      if(detectaApogeu == false){
          digitalWrite(IGN_1, HIGH);
          led2 = HIGH;
          led = HIGH;
          detectaApogeu = true;
          desligaLED = tempoAtual + intervalo;
          ligaLED2 = tempoAtual + intervalo2;
      }
      else if (led == LOW){
        dataString += String("subindo\t");
      }
    }

    if (detectaApogeu == true){

      if(led2 == HIGH){
       if(tempoAtual >= desligaLED){
          dataString += String("caindo\t");
          digitalWrite(IGN_1, LOW);
          led2 = LOW; 
       }
      }
    
      if(led3 == LOW && tempoAtual >ligaLED2 ){
          digitalWrite(IGN_2, HIGH);
          led4 = HIGH;
          led3 = HIGH;
          desligaLED2 = tempoAtual + desligaLED2;
      }
        
      if(led4 == HIGH){
         if (tempoAtual >= desligaLED2 ) {
            digitalWrite(IGN_2, LOW);
            led4 = LOW;
         }
      }
    }
  
    dataString += String(altRelativa);
    dataString += ("\t");

    dataString += String(mediaMovel);
    dataString += ("\t");

    dataString += String(segundaMediaMovel);
    dataString += ("\t");

    File dataFile = SD.open(arquivo, FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println(dataString);
    }
  // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening" + arquivo);
    }
      
    auxiliar = alt;
   
    // delay(100);
}
