#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float aux = 0;
float media = 0;
float soma = 0;
float mediaMovel = 0;
float segundaMediaMovel = 0;
float matriz[2][10];



void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
  
    Serial.begin(115200);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    }

   // Serial.print("Temp.(*C)\t");
   // Serial.print("Pres.(Pa)\t");
   // Serial.print("Alt.(m)\t");
   // Serial.print("Pres. sealevel(Pa)\t");
   // Serial.print("Real alt.(m)\t");
   // Serial.print("Foguete\t");
    Serial.print("Variação\t");
    Serial.print("Média\t");
    Serial.print("Média 2\t");
    Serial.println();

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
         matriz[1][i] = matriz[1][i+1];
     }
     matriz[1][9] = altRelativa;
  
    
     for (int i = 0; i < 10; i++){
        somaVet += matriz[1][i];
      }

    mediaMovel = somaVet/10.0;

    
      for (int i = 0; i < 9; i ++) {
        matriz [2][i] = matriz [2][i+1];
      }
      matriz[2][9] = mediaMovel;

      
      for (int i = 0; i < 10; i++){
        somaVet2 += matriz[2][i];
      }

    segundaMediaMovel = somaVet2/10.0;
    
    /* Serial.print(bmp.readTemperature());
    Serial.print("\t");
    
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    
    Serial.print(alt);
    Serial.print("\t");
   
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");

    if(alt > aux){
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
  
    Serial.print(altRelativa);
    Serial.print("\t");

    Serial.print(mediaMovel);
    Serial.print("\t");

    Serial.print(segundaMediaMovel);
    Serial.print("\t");
    
    Serial.println();
    
    aux = alt;
   
    // delay(100);
}
