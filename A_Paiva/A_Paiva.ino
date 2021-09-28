#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

//COLOCAR O MAIOR FILTRO PRIMEIRO
#define tam1 15                   //Tamanho do vetor do filtro1
#define tam2 10                   //Tamanho do vetor do filtro2
#define qf 2                      //Quantidade de filtros

float H11 = 0;                    //Valor da altitude atual do filtro1 (MediaMov)
float H12 = 0;                    //Valor da altitude atual do filtro2 

float H21 = 0;                    //Valor anterior ao do atual do filtro1
float H22 = 0;                    //Valor anterior ao do atual do filtro2

float Hmax1 = 0;                  //Valor máximo do filtro1
float Hmax2 = 0;                  //Valor máximo do filtro2

float SomaRef = 0;                //Soma valores iniciais(foguete parado na base)
float AltitudeRef = 0;            //É o valor da média dos valores iniciais(foguete parado na base)

float SomaMov1 = 0;               //Soma dos valores do vetor do filtro1
float SomaMov2 = 0;               //Soma dos valores do vetor do filtro2

float MediaMov1 = 0;              //É o valor da média dos valores do vetor do filtro1
float MediaMov2 = 0;              //É o valor da média dos valores do vetor do filtro2

float Delta1;                     //Diferença entre valor máximo do filtro1 (Hmax1) e valor atual referênciado (H11)
float Delta2;                     //Diferença entre valor máximo do filtro2 (Hmax2) e valor atual referênciado (H12)

float MatrizFiltros[qf][tam1];    //Vetor para guardar os valores para as médias utilizadas pelos filtros

int led = 0;                      //Variável para funcionamento do LED  

const int chipSelect = 53;

Adafruit_BMP085 bmp;              //Cria variável 'bmp' para a biblioteca Adafruit_BMP085

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  File dataFile = SD.open("Au.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Temperature(*C)\tPressure(Pa)\tAltitude(m)\tPressure at sealevel(calculated)(Pa)\tReal altitude(m)");
    dataFile.close();
    Serial.println("Temperature(*C)\tPressure(Pa)\tAltitude(m)\tPressure at sealevel(calculated)(Pa)\tReal altitude(m)");
  }
  for (int i = 0; i < 100; i++) {                       //Este 'for' serve para definir a altitude da base de lançamento como valor de referência.
    SomaRef = SomaRef + bmp.readAltitude();
  }
  AltitudeRef = SomaRef / 100;
}
void loop() {
  SomaMov1=0;                                           //Zera o SomaMov1 em todo loop
  SomaMov2=0;                                           //Zera o SomaMov2 em todo loop
  //Filtro 1
  for (int i = tam1-2; i>=0; i--){                      //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1
   MatrizFiltros[0][i+1]= MatrizFiltros[0][i];                    
  }
  MatrizFiltros[0][0]=bmp.readAltitude()-AltitudeRef;   //Esse é o valor mais atualizado do filtro1 
  for (int i = 0; i < tam1; i++) {                      //Esse 'for' faz a soma dos últimos valores medidos, para a média do filtro1
    SomaMov1=SomaMov1+MatrizFiltros[0][i];
  }
  MediaMov1=SomaMov1/tam1;                        
  H21 = H11;                                    
  H11 = MediaMov1;                              
  if (Hmax1 < H11) {                                    //Pega o valor máximo da média/filtro1
    Hmax1 = H11;
  }
  //Filtro 2
  for (int i = tam2-2; i>=0; i--){                      //Esse 'for' anda com os valores do vetor do filtro2 de 1 em 1
   MatrizFiltros[1][i+1]= MatrizFiltros[1][i];                    
  }
  MatrizFiltros[1][0]= H11;                             //Esse é o valor mais atualizado do filtro2 
  for (int i = 0; i < tam2; i++) {                      //Esse 'for' faz a soma dos últimos valores medidos, para a média do filtro2
    SomaMov2=SomaMov2+MatrizFiltros[1][i];
  }
  MediaMov2=SomaMov2/tam2;                        
  H22 = H12;                                    
  H12 = MediaMov2;                              
  if (Hmax2 < H12) {                                    //Pega o valor máximo da média/filtro2
    Hmax2 = H12;
  }

  Delta1=Hmax1-H11;                                     //Compara o valor máximo do filtro1 com o valor atual do filtro1
  Delta2=Hmax2-H12;                                     //Compara o valor máximo do filtro2 com o valor atual do filtro2

  File dataFile = SD.open("Au.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(Hmax2);
    dataFile.print("\t");
    dataFile.print(H11);
    dataFile.print("\t");
    dataFile.print(H12);
    dataFile.print("\t");
    dataFile.print(MatrizFiltros[0][0]);
    dataFile.print("\t");
    dataFile.close();
    Serial.print(Hmax2);
    Serial.print("\t");
    Serial.print(H11);
    Serial.print("\t");
    Serial.print(H12);
    Serial.print("\t");
    Serial.print(MatrizFiltros[0][0]);
    Serial.print("\t");
  }
  
  if (Delta2 >= 2) {                                    //Quando a diferença de altitude for acima de 2 (metros), provavelmente o foguete está descendo ou pode haver um controle de quando se quer que abra o paraquedas
    digitalWrite(LED_BUILTIN, HIGH);
    led = 1;
    Serial.print("Descendo");
    Serial.print("\t");
    Serial.print(led);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    led = 0;
    Serial.print("Subindo");
    Serial.print("\t");
    Serial.print(led);
  }
  Serial.println();
}
