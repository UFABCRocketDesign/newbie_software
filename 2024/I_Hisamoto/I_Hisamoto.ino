#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float AltitudeInicial;
float SomaAltitude;
float ListaSuavizarCurva_0[10];
float ListaSuavizarCurva_1[5];
float ListaDeteccaoQueda[2];
int contador = 0;


float filtroSuavizarCurva_0(float dadosCurva_0) {

  for (int i = 9; i > 0; i--) {
    ListaSuavizarCurva_0[i] = ListaSuavizarCurva_0[i - 1];
  }
  ListaSuavizarCurva_0[0] = dadosCurva_0;
  float SomaLista = 0;
  for (int i = 0; i < 10; i++) {
    SomaLista += ListaSuavizarCurva_0[i];
  }
  float MediaFiltro = SomaLista / 10;
  return MediaFiltro;
}

float filtroSuavizarCurva_1(float dadosCurva_1) {

  for (int i = 4; i > 0; i--) {
    ListaSuavizarCurva_1[i] = ListaSuavizarCurva_1[i - 1];
  }
  ListaSuavizarCurva_1[0] = dadosCurva_1;
  float SomaLista = 0;
  for (int i = 0; i < 5; i++) {
    SomaLista += ListaSuavizarCurva_1[i];
  }
  float MediaFiltro = SomaLista / 5;
  return MediaFiltro;
}


void setup() {

  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  Serial.print("Temperature(C)\t Pressure(Pa)\t High(meters)\t Fallen(1)/ Not fallen (0)\t Contador de Queda");

  SomaAltitude = 0;
  for (int posicaoListaAltitude = 0; posicaoListaAltitude < 10; posicaoListaAltitude++) {
    SomaAltitude += bmp.readAltitude();
  }

  AltitudeInicial = SomaAltitude / 10;
}

void loop() {
  
  float Altura = bmp.readAltitude() - AltitudeInicial;
  float Altura_Filtrada_0 = filtroSuavizarCurva_0(Altura);
  float Altura_Filtrada_1 = filtroSuavizarCurva_1(Altura_Filtrada_0);
  int fallenCondition = 0;

  for (int i = 1; i <= 2; i++){
    if (ListaDeteccaoQueda[i - 1] < ListaDeteccaoQueda[i]) {
      contador++;
    } else{ 
      contador = 0;
    }
    ListaDeteccaoQueda[0] = Altura_Filtrada_1;
    if (contador >= 5) {
      fallenCondition = 1;
    } else {
      fallenCondition = 0;
    }
  
  }

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
  Serial.print(fallenCondition);
  Serial.print("\t");
  Serial.print(contador);

  Serial.println();
}