#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float SomaAltitude;
float AltitudeInicial;
float SomaListaAltitude_0;
float MediaFiltroAltitude_0;
float ListaAltitude[10];

float filtro_0(){

  for(int i = 9; i > 0; i--){
    ListaAltitude[i] = ListaAltitude[i-1];
  }

  ListaAltitude[0] = bmp.readAltitude()- AltitudeInicial;
  
  SomaListaAltitude_0=0;
  for(int i = 0; i < 10; i++){
    SomaListaAltitude_0 += ListaAltitude[i];
  }

  MediaFiltroAltitude_0 = SomaListaAltitude_0/10;
  return MediaFiltroAltitude_0;

}

//float filtro_0(){
  //SomaListaAltitude_0=0;
  //for(int i=0;i<10;i++){
    //SomaListaAltitude_0 += bmp.readAltitude()- AltitudeInicial;
  //}
 // MediaFiltroAltitude_0 = SomaListaAltitude_0/10;
  //return MediaFiltroAltitude_0;
//}


void setup() {

  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Pressure at sealevel (calculated, Pa)\t Real altitude(meters)");

  SomaAltitude=0; 
  for(int posicaoListaAltitude=0; posicaoListaAltitude<10; posicaoListaAltitude++){
    SomaAltitude += bmp.readAltitude(); 
  }

  AltitudeInicial= SomaAltitude/10; 

}

void loop() {

  float Altura= bmp.readAltitude()/filtro_0();

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