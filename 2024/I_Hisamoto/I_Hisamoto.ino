#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

int SomaAltitude;
int AltitudeInicial;
int ListaAltitudeInicial[10];
int FiltroAltitude_1;

void setup() {

  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Pressure at sealevel (calculated, Pa)\t Real altitude(meters)");
  
  SomaAltitude=0; //para dar partida inicial a soma
  for(int posicaoListaAltitude=0; posicaoListaAltitude<10; posicaoListaAltitude++){
    SomaAltitude += bmp.readAltitude(); //soma das 10 primeiras leituras
  }

  AltitudeInicial= SomaAltitude/10; //media das 10 primeiras leituras

}

void loop() {

  for(int i=0; i<10; i++){ //nao soube definir um fim que não inteferisse nos dados ent usei um -0, neutro.
    ListaAltitudeInicial[i]= bmp.readAltitude();
    FiltroAltitude_1 += (ListaAltitudeInicial[i-1]+ListaAltitudeInicial[i+1])/2; //interpreta o prox valor como media do valor anterior + o valor seguinte.
  }

  float Altura= FiltroAltitude_1 - AltitudeInicial;

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