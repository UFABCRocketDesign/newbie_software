#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define tam 10                    //Tamanho da matriz do filtro
#define qf 2                      //Quantidade de filtros

float Hmax = 0;                   //Valor máximo filtrado

float SomaRef = 0;                //Soma valores iniciais(foguete parado na base)
float AltitudeRef = 0;            //É o valor da média dos valores iniciais(foguete parado na base)

float SomaMov = 0;                //Soma dos valores do vetor do filtro1

float MediaMov = 0;               //É o valor da média dos valores do vetor do filtro1

float Delta;                      //Diferença entre valor máximo do filtro1 (Hmax1) e valor atual referênciado (H11)

float MatrizFiltros[qf][tam];     //Vetor para guardar os valores para as médias utilizadas pelos filtros

int led = 0;                      //Variável para funcionamento do LED  

float T;
float P;
float Pm;

String NomeArq;
int aux = 1;
float x = 0.00001;

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
  while(aux==1){
    String y = String(x,5);
    String z = y.substring(2,5);
    NomeArq = "apm"+z+".txt";
    if (SD.exists(NomeArq)) {
      Serial.print(NomeArq);
      Serial.println(" ja existe");
      aux = 1;
    }
    else{
      File dataFile = SD.open(NomeArq, FILE_WRITE);
      if (dataFile) {
        dataFile.println("Temperatura(°C)\tPressao(Pa)\tPressao ao nivel do mar(Pa)\tAltura máxima(m)");
        for (int i = 0; i<qf; i++){
          dataFile.print("Altura do filtro ");
          dataFile.print(i);
          dataFile.print("(m)\t");
        }
        dataFile.println("Statu de voo");
        dataFile.close();
      }
      Serial.println("Dados dealtitude de voo");
      Serial.print("Temperatura(°C)\tPressao(Pa)\tPressao ao nivel do mar(Pa)\tAltura máxima(m)\tAltura (m)\tStatu de voo");
      for (int i = 0; i<qf; i++){
        Serial.print("Altura do filtro ");
        Serial.print(i);
        Serial.print("(m)\t");
      }
      Serial.println("Statu de voo");
      for (int i = 0; i < 100; i++) {                       //Este 'for' serve para definir a altitude da base de lançamento como valor de referência.
        SomaRef = SomaRef + bmp.readAltitude();
      }
      AltitudeRef = SomaRef / 100;
      aux = 0;
    }
    x = x+0.00001;
  }
}
void loop() {
  T = bmp.readTemperature();
  P = bmp.readPressure();
  Pm = bmp.readSealevelPressure();
  File dataFile = SD.open(NomeArq, FILE_WRITE);
  if (dataFile) {
    dataFile.print(T);
    dataFile.print("\t");
    dataFile.print(P);
    dataFile.print("\t");
    dataFile.print(Pm);
    dataFile.print("\t");    
    dataFile.print(Hmax);
    dataFile.print("\t");
  }
  Serial.print(T);
  Serial.print("\t");
  Serial.print(P);
  Serial.print("\t");
  Serial.print(Pm);
  Serial.print("\t");
  Serial.print(Hmax);
  Serial.print("\t");
  SomaMov=0;                                           //Zera o SomaMov1 em todo loop
  for (int j = 0; j<qf; j++){
      for (int i = tam-2; i>=0; i--){                      //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1
        MatrizFiltros[j][i+1]= MatrizFiltros[j][i];                    
      }
      if(j==0){
        MatrizFiltros[0][0]=bmp.readAltitude()-AltitudeRef;   //Esse é o valor mais atualizado do filtro1
        Serial.print(MatrizFiltros[0][0]);
        Serial.print("\t");
        if (dataFile) {
          dataFile.print(MatrizFiltros[0][0]);
          dataFile.print("\t");
        }
      }
      else{
        MatrizFiltros[j][0] = MediaMov;
      }
      SomaMov=0;
      for (int i = 0; i <= tam-1; i++) {                      //Esse 'for' faz a soma dos últimos valores medidos, para a média do filtro1
        SomaMov=SomaMov+MatrizFiltros[j][i];
      }
      MediaMov=SomaMov/tam;
      Serial.print(MediaMov);
      Serial.print("\t");
      if (dataFile) {
        dataFile.print(MediaMov);
        dataFile.print("\t");
      }
  }                    
  if (Hmax < MediaMov) {                                    //Pega o valor máximo da média/filtro2
    Hmax = MediaMov;
  }
  Delta=Hmax-MediaMov;                                     //Compara o valor máximo do filtro1 com o valor atual do filtro1
  if (Delta >= 2) {                                    //Quando a diferença de altitude for acima de 2 (metros), provavelmente o foguete está descendo ou pode haver um controle de quando se quer que abra o paraquedas
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
