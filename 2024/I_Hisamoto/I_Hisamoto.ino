#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float AltitudeInicial;
float ListaAltitude[10];
int SomaAltitude;
int MediaAltitudes;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Pressure at sealevel (calculated, Pa)\t Real altitude(meters)");
}

void loop() {

  ListaAltitude[0]= bmp.readAltitude(); //armazena valores de altitude

  SomaAltitude=0; //para dar partida inicial a soma
  for(int posicaoListaAltitude=0; posicaoListaAltitude<10; posicaoListaAltitude++){
    SomaAltitude += ListaAltitude[posicaoListaAltitude]; //soma das 10 primeiras leituras
  }

  MediaAltitudes= SomaAltitude/10; //media das 10 primeiras leituras
  float Altura= bmp.readAltitude()- MediaAltitudes; 

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