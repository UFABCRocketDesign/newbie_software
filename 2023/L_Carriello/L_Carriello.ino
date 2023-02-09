#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;


float alt_inicial;
float soma;
int i;
int j = 0;
float sum = 0;
int valores[5];
float media = 0;
float altura = 0;
float altura_sem_ruido = 0;
float alt0 = 0;


void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  Serial.print("Temperature(C) \t");
  Serial.print("Pressure at sealevel(Pa) \t");
  Serial.print("Altitude(m) \t");
  Serial.print("Altitude sem ruído(m) \t");
  Serial.println();

  for (i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
  }

  alt_inicial = soma / 10;
}


void loop() {

  altura = bmp.readAltitude();
  alt0 = altura - alt_inicial;
  for(j=0; j < 5; j++){
    valores[j] = altura;
    sum += valores[j+1];
    valores[j] = valores[j+1];
    
     if(j == 5){
      sum += valores[j];
      sum -= valores[0];
    }
  }
 
  media = sum/j;
  altura_sem_ruido = altura - media;
  
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(alt0);
  Serial.print("\t");
  Serial.print(altura_sem_ruido);
  Serial.print(sum);

  Serial.println();
  delay(10);
}