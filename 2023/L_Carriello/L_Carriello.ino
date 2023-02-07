#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;

#define nmax 5

float alt_inicial;
float soma;
int i;
int index = 0;
float valor = 0;
float sum = 0;
int leituras[nmax];
float media = 0;
int j = 0;
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

  for(j; j < nmax; j++){
    valor = bmp.readAltitude();
    leituras[index] = valor;
    sum += valor;

     if(index == nmax){
      sum += leituras[index];
      sum -= leituras[0];
    }
  }
 
  media = sum/nmax;
  altura_sem_ruido = bmp.readAltitude() - media;
  
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude() - alt_inicial);
  Serial.print("\t");
  Serial.print(altura_sem_ruido);

  Serial.println();
  delay(10);
}