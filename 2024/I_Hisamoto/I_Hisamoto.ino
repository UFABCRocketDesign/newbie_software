#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float SomaAltitude;
float AltitudeInicial;
float SomaLista_0;
float SomaLista_1;
float MediaFiltro_0;
float MediaFiltro_1;
float Lista_0[10];
float Lista_1[5]

float filtro_0(float var_0) {

  for (int i = 9; i > 0; i--) {
    Lista_0[i] = Lista_0[i - 1];
  }

  Lista_0[0] = var;
  SomaLista_0 = 0;

  for (int i = 0; i < 10; i++) {
    SomaLista_0 += Lista_0[i];
  }

  MediaFiltro_0 = SomaLista_0 / 10;

  return MediaFiltro_0;
}

float filtro_1(float var_1) {

  for (int i = 4; i > 0; i--) {
    Lista_1[i] = Lista_1[i - 1];
  }

  Lista_1[0] = var_1;
  SomaLista_0 = 0;

  for (int i = 0; i < 5; i++) {
    SomaLista_0 += Lista_1[i];
  }

  MediaFiltro_1 = SomaLista_1 / 5;

  return MediaFiltro_1;
}

void setup() {

  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Pressure at sealevel (calculated, Pa)\t Real altitude(meters)");

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
  Serial.print(Altura_Filtrada_1);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));

  Serial.println();
}