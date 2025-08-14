#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alt_zero = 0;              
float vetor_num [10];
int ind_vetor = 0;
float valor_flt = 0;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {           
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) { }               
  }
  float soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
                      
  }
  alt_zero = soma/10.0;
}

void loop() {
  float real_alt = bmp.readAltitude() - alt_zero;
  
  vetor_num [ind_vetor] = real_alt;
  ind_vetor ++;
  if (ind_vetor >= 10) ind_vetor = 0;
    
  float valor_vet = 0;
  for (int i = 0; i < 10 ; i++) {
   valor_vet += vetor_num [i];
  }
  valor_flt = valor_vet / 10;

  Serial.print(bmp.readTemperature());       
  Serial.print('\t');
  Serial.print(bmp.readPressure());          
  Serial.print('\t');
  Serial.print(real_alt);
  Serial.print('\t');
  Serial.print(valor_flt);  
  Serial.print('\t');
  Serial.println();

}
