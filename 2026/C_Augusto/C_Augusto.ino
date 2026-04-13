#include <Adafruit_BMP085.h>
//#include <Adafruit_BMP280.h>
//#include <Wire.h>
#include <SD.h>

// SD
#define SD_CS_PIN 53;
#define SD_CS_PIN 10;
const int chipSelect = 53;
char charSD[15];
int numtxtSD = 0;

//detecção de altura
float altura0;
float alturasoma = 0;
float alturatarada;

//filtragem de dados
float alpha = 0.2;   // fator de suavização
float filtrado = 0;  // valor inicial
float filtrado2 = 0; //valor inicial2
float filtrado3 = 0;
float apogeu = 0;
float aux = 0;

//detecção de queda
int det_queda;
int mediafiltro[8];
int queda;
int resultado;

Adafruit_BMP085 bmp;
//Adafruit_BMP280 bmp;
  
void setup() {
  int contador = 0;
  Serial.begin(115200);
  //delay(1000);
  //Wire.begin(21,22);
  if (!bmp.begin()) {
	Serial.println("bmp nao encontrado, verifique as conexoes");
	while (1) {}
  }
  //Serial.println("Iniciando SD");

  //if(!SD.begin(chipSelect)){
    //Serial.println("erro ao iniciar SD");
    //while (true);
  //}

  //File dataFile = SD.open(charSD, FILE_WRITE);
  //while (true){
   //sprintf(charSD, numtxtSD == 0 ? "dataCA.txt" : "dataCA%03d.txt", numtxtSD);
   //if(!SD.exists(charSD)) break;
  //numtxtSD++;
  //}
  Serial.println("Temperatura   |   Pressao   |   Altitude    |   Filtro    |   Apogeu    ");
  for(int i = 0; i < 9; i++){
    alturasoma += bmp.readAltitude();
  }
  alturasoma = alturasoma/9; 
}
  
void loop() {
  float alturatarada = bmp.readAltitude() - alturasoma;

  filtrado = alpha * alturatarada + (1 - alpha) * aux;
  aux = filtrado;
  
  filtrado2 = alpha * filtrado + (1-alpha) * filtrado2;
  filtrado3 = alpha * filtrado2 + (1-alpha) * filtrado3;
  
if (filtrado3 > apogeu) {
  apogeu = filtrado3;
}

for(int i= 8-1; i > 0; i++){
  mediafiltro[i] = mediafiltro[i-1];
}
  mediafiltro[0] = filtrado3;

for(int i = 0; i < 8; i++){
  resultado += mediafiltro[i];
}
resultado = resultado/8;

if (resultado < mediafiltro){
  queda = 1;
} else{
  queda = 0;
}

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(alturatarada);
    Serial.print("\t");
    Serial.print(filtrado);
    Serial.print("\t");
    Serial.print(filtrado2);
    Serial.print("\t");
    Serial.print(filtrado3);
    Serial.print("\t");
    Serial.print(resultado);
    Serial.print("\t");
    Serial.print(apogeu);
    Serial.print("\t");
    Serial.print(queda);
    
    Serial.println();
}