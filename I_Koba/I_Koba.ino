#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define chipSelect 53
#define led 13
#define pmt 20 // intervalo de filtros  
#define nf (3)  // Numero de filtros 
#define ncp 4  // intervalo de comparação para queda e suibida


Adafruit_BMP085 bmp;

float z, ref_chao;
float vetor[nf][pmt]; // movimentaçãop dos filtros de sinal de alrura 
float sinal[nf+1];   // irá conter todos sinais relacionado a altura  
float sinalzin[ncp]; // contem os dados usados para comparar a altura 
String Dados_string = ""; // irá conter os dados dos sinais em string
String var_queda;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }

  Serial.print("Temp (*C)\t");
  Serial.print("Pres (Pa)\t");
  Serial.print("Alt (m)\t");
   
  //media para referenciar a altura
  call_chao();

  Serial.println();
}

void loop() {

  // Serial.print(bmp.readTemperature());
  // Serial.print("\t");
  // Serial.print(bmp.readPressure());
  // Serial.print("\t");

  
  sinal[0] = bmp.readAltitude() - ref_chao;
  Filtros();

  String Dados_string = "";
  for (int y = 0; y < nf+1; y++) {
    Dados_string = String(sinal[y]);
    Dados_string += "\t";
  }
    Serial.print(Dados_string);
    Serial.print("\t");
    
  if (detec_queda()){
    var_queda = "1";
    Serial.print(var_queda);
  }else{
    var_queda = "0";
    Serial.print(var_queda);
  }
  Serial.println();
  salvar();
}

//-----------------------------------------------------------------------------
void call_chao() {
  z = 0;
  for (int x = 0; x < 100; x++) {
    z = bmp.readAltitude() + z;
  }
  ref_chao = z / 100;

   //salvando a constante que refencia o chão.
   File dataFile = SD.open("I_Koba.txt", FILE_WRITE);
   dataFile.print("Alt(m)\t");
   dataFile.print("detecção de queda");
   dataFile.print("constante que referencia o chão = ");
   dataFile.println(ref_chao);
   dataFile.close();
 
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
    sinal[y + 1] = k / pmt;
  }
}

//----------------------------------------------------------------------------------
bool detec_queda() {
  for (int x = ncp - 1; x > 0; x--) {
      sinalzin[x] = sinalzin[x - 1];
    }
  sinalzin[0] = sinal[nf];
  return (sinalzin[0] > sinalzin[ncp - 1]);
    }

//----------------------------------------------------------------------------------
void salvar(){ 
   File dataFile = SD.open("I_Koba.txt", FILE_WRITE);
   dataFile.print(Dados_string);
   dataFile.print("\t");
   dataFile.print(var_queda);
   dataFile.print("\t");
   dataFile.close();
}

//----------------------------------------------------------------------------------
