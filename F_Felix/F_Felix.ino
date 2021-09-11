#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float aux = 0;
float media = 0;
float soma = 0;

void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
  
    Serial.begin(115200);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    }

    Serial.print("Temp.(*C)\t");
    Serial.print("Pres.(Pa)\t");
    Serial.print("Alt.(m)\t");
    Serial.print("Pres. sealevel(Pa)\t");
    Serial.print("Real alt.(m)\t");
    Serial.print("Foguete\t");
    Serial.print("Variação\t");
    Serial.println();

    float alt = bmp.readAltitude();

    for(int i = 0; i < 100; i++){
       soma += alt; 
    }
    
    media = soma/100;

}

// the loop function runs over and over again forever
void loop() { 
    
    float alt = bmp.readAltitude();
    
    int altRelativa = media - alt;
    
    Serial.print(bmp.readTemperature());
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
    }

    Serial.print(altRelativa);
    Serial.print("\t");
    
    Serial.println();
    
    aux = alt;
   
    // delay(100);
}
