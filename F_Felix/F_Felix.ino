#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;
float aux = 0;
float media = 0;
float soma = 0;
float mediaMovel = 0;
float segundaMediaMovel = 0;
float matriz[2][10];
String arquivo = "";
int num = 0;

const int chipSelect = 53;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

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

    dataString += ("Real alt.(m)\t");
    
    String nome = "felix";
    int cont = 0;
    String txt = ".txt";
    bool existente = false;

  while(existente == false)
    if(num < 100){
      arquivo = nome + String(num) + txt;
      
      if(SD.exists(arquivo)) {
        num = num + 1;
        existente = false;
      }
      else {
        File dataFile = SD.open(arquivo, FILE_WRITE);
        dataFile.println(dataString);
        dataFile.close();
        existente = true;
      }
    }

   // Serial.print("Temp.(*C)\t");
   // Serial.print("Pres.(Pa)\t");
   // Serial.print("Alt.(m)\t");
  //  Serial.print("Pres. sealevel(Pa)\t");
  //  Serial.print("Real alt.(m)\t");
  //  Serial.print("Foguete\t");
  //  Serial.print("Variação\t");
  //  Serial.print("Média\t");
  //  Serial.print("Média 2\t");
  //  Serial.println();

    float alt = bmp.readAltitude();

    for (int j = 0; j < 100; j++){
      soma += alt;
    }

    media = soma/100.0;
}

// the loop function runs over and over again forever
void loop() { 
    
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
     
  
   // Serial.print(bmp.readTemperature());
  //  Serial.print("\t");
    
  //  Serial.print(bmp.readPressure());
  //  Serial.print("\t");
    
   // Serial.print(alt);
  //  Serial.print("\t");
   
  //  Serial.print(bmp.readSealevelPressure());
  //  Serial.print("\t");
  //  
  //  Serial.print(bmp.readAltitude(101500));
  //  Serial.print("\t");

    /*if(alt > aux){
      Serial.print("subindo\t");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (alt == aux){
      Serial.print("parado\t");
    }
    else{
      Serial.print("caindo\t");
      digitalWrite(LED_BUILTIN, HIGH);
    } */
  
  //  Serial.print(altRelativa);
  //  Serial.print("\t");

  //  Serial.print(mediaMovel);
  //  Serial.print("\t");

  //  Serial.print(segundaMediaMovel);
  //  Serial.print("\t");
    
 //   Serial.println();

    String dataString = "";
    
    dataString+=String(bmp.readTemperature());
    dataString+= ("\t");

    dataString+=String(bmp.readPressure());
    dataString+= ("\t");

    dataString+=String(bmp.readAltitude(101500));
    dataString+= ("\t");

    dataString += String(altRelativa);
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
      
    aux = alt;
   
    // delay(100);
}
