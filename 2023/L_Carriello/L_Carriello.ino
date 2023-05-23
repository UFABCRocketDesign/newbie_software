#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 53;
Adafruit_BMP085 bmp;

#define n 10
#define n2 5

float alt_inicial;
float soma;
int i;
float num[n];  //vetor c/valores pra média móvel
float num2[n];
float num3[n2];
float altura_sem_ruido;
float altura = 0;
int queda;

  float filtro(float media){
    for(i = n-1; i>0; i--) num[i] = num[i-1];
    num[0] = media;

    float acc = 0; //soma os pontos da média móvel
    for(i=0; i<n; i++) acc += num[i]; //somatória do número de pontos

    return acc/n; //retorna média móvel

  }

  float filtro2(float media2){
    for(i = n-1; i>0; i--) num2[i] = num2[i-1];
    num2[0] = media2;

    float acc2 = 0; //soma os pontos da média móvel
    for(i=0; i<n; i++) acc2 += num2[i]; //somatória do número de pontos

    return acc2/n; //retorna média móvel

  }


void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  String dataString = "";

  dataString += "Temperature(C) \t";
  dataString += "Pressure at sealevel(Pa) \t";
  dataString += "Altitude(m) \t";
  dataString += "Altitude filtrada 1(m) \t";
  dataString += "Altitude filtrada 2(m) \t";
  dataString += "Detecção de queda \t";
  
  //imprimindo o cabeçalho
  Serial.println(dataString);

  for (i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
  }

  alt_inicial = soma / 10;

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

 // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  File dataFile = SD.open("lais.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);    
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening lais.txt");
  }

}


void loop() {

  float temperatura = bmp.readTemperature();
  float pressao = bmp.readSealevelPressure();
  float altura = bmp.readAltitude()- alt_inicial;
  float altura_sem_ruido = filtro(altura);
  float alt_filtrada2 = filtro2(altura_sem_ruido);
  int queda;

  for(i = n2-1; i>0; i--){
    num3[i] = num3[i-1];
  }
  num3[0] = alt_filtrada2;
  
  if (num3[0]<num3[1] && num3[1]<num3[2] && num3[2]<num3[3] && num3[3]<num3[4]){
    queda = 1;
  }
  else{
    queda = 0;
  }

  

// make a string for assembling the data to log:
  String dataString = "";
  dataString += String(temperatura);
  dataString += "\t";
  dataString += String(pressao);
  dataString += "\t";
  dataString += String(altura);
  dataString += "\t";
  dataString += String(altura_sem_ruido);
  dataString += "\t";
  dataString += String(alt_filtrada2);
  dataString += "\t";
  dataString += String(queda);
  
  Serial.println(dataString);

  File dataFile = SD.open("lais.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);    
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening lais.txt");
  }

  Serial.println();
  delay(10);

}