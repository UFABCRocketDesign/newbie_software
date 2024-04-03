#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float SomaAltitude;
float AltitudeInicial;
float SomaLista_0;
float MediaFiltro_0;
float Lista[10];

float filtro_0(float var) {

  for (int i = 9; i > 0; i--) {
    Lista[i] = Lista[i - 1];
  }

  Lista[0] = var;
  SomaLista_0 = 0;

  for (int i = 0; i < 10; i++) {
    SomaLista_0 += Lista[i];
  }

  MediaFiltro_0 = SomaLista_0 / 10;

  return MediaFiltro_0;
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
  
  float Altura_Filtrada = filtro_0(bmp.readAltitude() - AltitudeInicial);

  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(Altura_Filtrada);
  Serial.print("\t");
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
  Serial.print(bmp.readAltitude(101500));

  Serial.println();
}