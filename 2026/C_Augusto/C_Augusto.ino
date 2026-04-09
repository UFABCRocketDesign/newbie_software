#include <Adafruit_BMP085.h>
float altura0;
float alturasoma = 0;
float alturatarada;
float RS = 0.05; //ruido fisico
float RM = 0.3; //ruido da medicao
float c = 1.0; //confiança do filtro
float g = 0;//ganho do filtro
float filtrocalman = 0;
float lowpass = 0;
float lowpass_suavizacao = 0.1;
float apogeu = 0;


Adafruit_BMP085 bmp;
  
void setup() {
  int contador = 0;
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   Filtro    |   Apogeu    ");
  for(int i = 0; i < 9; i++){
    alturasoma += bmp.readAltitude();
  }
  alturasoma = alturasoma/9; 
}
  
void loop() {
  float alturatarada = bmp.readAltitude() - alturasoma;

  //algoritmo do meu mano calman
  c = c + RS;
  g = c/(c+RM);
  filtrocalman = filtrocalman + g*(alturatarada - filtrocalman);
  c = (1-g)*c;
  lowpass = (filtrocalman*lowpass_suavizacao) + (lowpass*(1.0-lowpass_suavizacao))*(lowpass*(1.0-lowpass_suavizacao))/2;

if (lowpass > apogeu) {
  apogeu = lowpass;
}

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.print("\t");
    Serial.print(lowpass);
    Serial.print("\t");
    Serial.print(apogeu);
    Serial.println();
}