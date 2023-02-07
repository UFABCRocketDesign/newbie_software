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

  for(j=0; j < 5; j++){
    valores[j] = bmp.readAltitude();
    valores[j] = valores[j+1];
    sum += valores[j+1];

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
  Serial.print(altura - alt_inicial);
  Serial.print("\t");
  Serial.print(altura_sem_ruido);

  Serial.println();
  delay(10);
}