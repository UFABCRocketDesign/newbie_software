#include <Adafruit_BMP085.h>
#define led 13
#define pmt 20 // intervalo de filtros  
#define nf 3  // Numero de filtros
#define ncp 4

Adafruit_BMP085 bmp;
float z, const_chao;
float vetor[nf][pmt];
float sinal[nf];
float sinalzin[ncp];

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  //media para referenciar a altura
  call_chao();

  Serial.print("Temp (*C)\t");
  Serial.print("Pres (Pa)\t");
  Serial.print("Alt (m)\t");
  Serial.println();
}

void loop() {

  // Serial.print(bmp.readTemperature());
  // Serial.print("\t");
  // Serial.print(bmp.readPressure());
  // Serial.print("\t");


  sinal[0] = bmp.readAltitude() - const_chao;
  Filtros();
  detec_queda();
  
  for (int y = 0; y < nf; y++) {
    Serial.print(sinal[y]);
    Serial.print("\t");
  }
  Serial.println();
}

//-----------------------------------------------------------------------------
void call_chao() {
  z = 0;
  for (int x = 0; x < 100; x++) {
    z = bmp.readAltitude() + z;
  }
  const_chao = z / 100;
}
//----------------------------------------------------------------------------------
void Filtros() {
  for (int y = 0; y < nf; y++) {
    for (int x = pmt - 1; x > 0; x--) {
      vetor[y][x] = vetor[y][x - 1];
    }
    vetor[y][0] = sinal[y];
    float k = 0;
    for (int x = 0; x < pmt; x++) {
      k = vetor[y][x] + k;
    }
    if ((nf+1)!= y){
      sinal[y + 1] = k / pmt;
    }
  }
}

//----------------------------------------------------------------------------------
void detec_queda() {
  for (int x = ncp - 1; x > 0; x--) {
      sinalzin[x] = sinalzin[x - 1];
    }
  sinalzin[0] = sinal[nf];
  if (sinalzin[0] > sinalzin[ncp - 1]){
    Serial.print("subindo");
  }else {
    Serial.print("caindo");
  }

}

//----------------------------------------------------------------------------------

//void media_movel() {
//
// for(int x = pmt-1; x > 0; x--){
//   vetor[0][x]= vetor[0][x-1];
// }
//vetor[0][0]= bmp.readAltitude() - const_chao;
// float k = 0;
// for(int x = 0; x < pmt; x++){
//   k = vetor[0][x] + k;
// }
//sinal_filtrado = k/pmt;
//}

//----------------------------------------------------------------------------------

//void media_movel_2() {
//
// for(int x = pmt-1; x > 0; x--){
//   vetor[1][x]= vetor[1][x-1];
// }
// vetor[1][0]= sinal_filtrado ;
// float k = 0;
// for(int x = 0; x < pmt; x++){
//   k = vetor[1][x] + k;
// }
// sinal_filtrado_2 = k/pmt;
//}

//----------------------------------------------------------------------------------
