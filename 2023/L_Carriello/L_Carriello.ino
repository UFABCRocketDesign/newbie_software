#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;

#define n 10

float alt_inicial;
float soma;
int i;
float num[n];  //vetor c/valores pra média móvel
float num2[n];
float altura_sem_ruido;
float altura = 0;


  float filtro(float media){
    for(i = n-1; i>0; i--) num[i] = num[i-1];
    num[0] = media;

    float acc = 0; //soma os pontos da média móvel
    for(i=0; i<n; i++) acc += num[i]; //somatória do número de pontos

    return acc/n; //retorna média móvel

  }

  float filtro2(float media2){
    for(i = n-1; i>0; i--) num2[i] = num2[i-1];
    num2[0] = media2;

    float acc2 = 0; //soma os pontos da média móvel
    for(i=0; i<n; i++) acc2 += num2[i]; //somatória do número de pontos

    return acc2/n; //retorna média móvel

  }


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

float alturo_sem_ruido;

void loop() {

  altura = bmp.readAltitude()- alt_inicial;
  
  
  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");

  Serial.print(altura);
  Serial.print("\t");

  altura_sem_ruido = filtro(altura);
  Serial.print(altura_sem_ruido);
  
  float alt_filtrada2 = filtro2(altura_sem_ruido);
  Serial.print(altura_sem_ruido);

  Serial.println();
  delay(10);
}