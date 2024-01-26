#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float SomaAltitude;
float AltitudeInicial;
float ListaAltitude_0[10];
float SomaListaAltitude_0;
float FiltroAltitude_0[10];
float SomaFiltroAltitude_0;

void setup() {

  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Pressure at sealevel (calculated, Pa)\t Real altitude(meters)");
  
  //determina uma altitude inicial baseada na media das 10 primeiras leituras
  SomaAltitude=0; 
  for(int posicaoListaAltitude=0; posicaoListaAltitude<10; posicaoListaAltitude++){
    SomaAltitude += bmp.readAltitude(); 
  }

  AltitudeInicial= SomaAltitude/10; 

}

void loop() {

  //cria uma lista para as leituras da altitude e outra lista para a soma dessas leituras. 
  SomaListaAltitude_0=0;
  for(int i=0; i<10; i++){ 
    ListaAltitude_0[i]= bmp.readAltitude()- AltitudeInicial;
    SomaListaAltitude_0 += ListaAltitude_0[i];
    FiltroAltitude_0[i]= ListaAltitude_0[i];
    SomaFiltroAltitude_0 += FiltroAltitude_0[i];
  }

  // determina a altura a partir da divisão da leitura atual pela media das ultimas 10 leituras.
  float MediaFiltroAltitude_0= SomaFiltroAltitude_0/10;
  float Altura= bmp.readAltitude()/MediaFiltroAltitude_0;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(Altura);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
    
  Serial.println();
}