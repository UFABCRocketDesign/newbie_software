#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

//COLOCAR O MAIOR FILTRO PRIMEIRO
#define tam 10                   //Tamanho da matriz do filtro
#define qf 2                      //Quantidade de filtros

/*float H11 = 0;                    //Valor da altitude atual do filtro1 (MediaMov)
float H12 = 0;                    //Valor da altitude atual do filtro2 

float H21 = 0;                    //Valor anterior ao do atual do filtro1
float H22 = 0;                    //Valor anterior ao do atual do filtro2

float Hmax1 = 0;                  //Valor máximo do filtro1*/
float Hmax = 0;                  //Valor máximo filtrado

float SomaRef = 0;                //Soma valores iniciais(foguete parado na base)
float AltitudeRef = 0;            //É o valor da média dos valores iniciais(foguete parado na base)

float SomaMov = 0;               //Soma dos valores do vetor do filtro1
//float SomaMov2 = 0;               //Soma dos valores do vetor do filtro2

float MediaMov = 0;              //É o valor da média dos valores do vetor do filtro1
//float MediaMov2 = 0;              //É o valor da média dos valores do vetor do filtro2

float Delta;                     //Diferença entre valor máximo do filtro1 (Hmax1) e valor atual referênciado (H11)
//float Delta2;                     //Diferença entre valor máximo do filtro2 (Hmax2) e valor atual referênciado (H12)

float MatrizFiltros[qf][tam];    //Vetor para guardar os valores para as médias utilizadas pelos filtros

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
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");
  File dataFile = SD.open("Au.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Altura máxima(m)\tAltura Filtrada(m)\tStatu de voo");
    dataFile.close();
  }
  Serial.println("Dados dealtitude de voo");
  Serial.println("Altura máxima(m)\tAltura dos Filtros(m)\tStatu de voo");
  for (int i = 0; i < 100; i++) {                       //Este 'for' serve para definir a altitude da base de lançamento como valor de referência.
    SomaRef = SomaRef + bmp.readAltitude();
  }
  AltitudeRef = SomaRef / 100;
}
void loop() {
  File dataFile = SD.open("Au.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(Hmax);
    dataFile.print("\t");
    //dataFile.close();
  }
  Serial.print(Hmax);
  Serial.print("\t");
  SomaMov=0;                                           //Zera o SomaMov1 em todo loop
  //SomaMov2=0;                                           //Zera o SomaMov2 em todo loop
  for (int j = 0; j<=qf-1; j++){
      for (int i = tam-2; i>=0; i--){                      //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1
        MatrizFiltros[j][i+1]= MatrizFiltros[j][i];                    
      }
      if(j==0){
        MatrizFiltros[0][0]=bmp.readAltitude()-AltitudeRef;   //Esse é o valor mais atualizado do filtro1 
      }
      else{
        MatrizFiltros[j][0] = MediaMov;
      }
      for (int i = 0; i <= tam-1; i++) {                      //Esse 'for' faz a soma dos últimos valores medidos, para a média do filtro1
        SomaMov=SomaMov+MatrizFiltros[j][i];
      }
      MediaMov=SomaMov/tam;
      Serial.print(MediaMov);
      Serial.print(" ");
  }
  Serial.print("\t");
  //File dataFile = SD.open("Au.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(MediaMov);
    dataFile.print("\t");
    //dataFile.close();
  }
  /*for (int i = tam1-2; i>=0; i--){                      //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1
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
  H12 = MediaMov2;*/                             
  if (Hmax < MediaMov) {                                    //Pega o valor máximo da média/filtro2
    Hmax = MediaMov;
  }

  Delta=Hmax-MediaMov;                                     //Compara o valor máximo do filtro1 com o valor atual do filtro1
  //Delta2=Hmax2-H12;                                     //Compara o valor máximo do filtro2 com o valor atual do filtro2

  /*File dataFile = SD.open("Au.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(Hmax2);
    dataFile.print("\t");
    dataFile.print(H11);
    dataFile.print("\t");
    dataFile.print(H12);
    dataFile.print("\t");
    dataFile.print(MatrizFiltros[0][0]);
    dataFile.println("\t");
    dataFile.close();
  }

    Serial.print(Hmax2);
    Serial.print("\t");
    Serial.print(H11);
    Serial.print("\t");
    Serial.print(H12);
    Serial.print("\t");
    Serial.print(MatrizFiltros[0][0]);
    Serial.print("\t");*/
  
  if (Delta >= 2) {                                    //Quando a diferença de altitude for acima de 2 (metros), provavelmente o foguete está descendo ou pode haver um controle de quando se quer que abra o paraquedas
    //File dataFile = SD.open("Au.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println("Descendo");
      dataFile.close();
    }
    digitalWrite(LED_BUILTIN, HIGH);
    led = 1;
    Serial.print("Descendo");
    Serial.print("\t");
    Serial.print(led);
  }
  else {
    //File dataFile = SD.open("Au.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println("Subindo");
      dataFile.close();
    }
    digitalWrite(LED_BUILTIN, LOW);
    led = 0;
    Serial.print("Subindo");
    Serial.print("\t");
    Serial.print(led);
  }
  Serial.println();
}
