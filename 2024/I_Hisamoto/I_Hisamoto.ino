#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float AltitudeInicial;
float SomaAltitude;
float Lista_0[10];
float Lista_1[5];
float Lista_2[5];

float filtro_0(float var_0) {

  for (int i = 9; i > 0; i--) {
    Lista_0[i] = Lista_0[i - 1];
  }

  Lista_0[0] = var;
  float SomaLista = 0;

  for (int i = 0; i < 10; i++) {
    SomaLista += Lista_0[i];
  }

  float MediaFiltro = SomaLista / 10;

  return MediaFiltro;
}

float filtro_1(float var_1) {

  for (int i = 4; i > 0; i--) {
    Lista_1[i] = Lista_1[i - 1];
  }

  Lista_1[0] = var_1;
  float SomaLista = 0;

  for (int i = 0; i < 5; i++) {
    SomaLista += Lista_1[i];
  }

  float MediaFiltro = SomaLista / 5;

  return MediaFiltro;
}

float Fall(float var){

  int contador = 0;
  int FallenCondition = 0;

  for (int i = 5; i > 0; i--) {
    Lista_2[i] = Lista_2[i - 1];
    Lista_2[0] = var;

    if (Lista_2[i-1]<Lista_2[i]){
      contador++;
      
      if (contador==5){
        FallenCondition = 1;
      }
      else{
        FallenCondition = 0;
      }
    }
  }

  return FallenCondition;

}

void setup() {

  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Pressure at sealevel (calculated, Pa)\t Real altitude(meters)\t Fallen(1)/ Not fallen (0)");

  SomaAltitude = 0;
  for (int posicaoListaAltitude = 0; posicaoListaAltitude < 10; posicaoListaAltitude++) {
    SomaAltitude += bmp.readAltitude();
  }

  AltitudeInicial = SomaAltitude / 10;
}

void loop() {
  
  float Altura = bmp.readAltitude() - AltitudeInicial;
  float Altura_Filtrada_0 = filtro_0(Altura);
  float Altura_Filtrada_1 = filtro_1(Altura_Filtrada_0);

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(Altura);
  Serial.print("\t");
  Serial.print(Altura_Filtrada_0);
  Serial.print("\t");
  Serial.print(Altura_Filtrada_1);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));
  Serial.print("\t");
  Serial.print(Fall(Altura_Filtrada_1));


  Serial.println();
}