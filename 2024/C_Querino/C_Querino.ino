#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;
const int chipSelect = 53;

/*#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif // ARDUINO_AVR_MEGA2560*/
//altura menos 3
#define IGN_1 36	/*act1*/
#define IGN_2 61	/*act2*/
#define IGN_3 46	/*act3*/
#define IGN_4 55	/*act4*/

float alturainicial;
float altura = 0;
float height[20];
float altufinal;
float somaH;
float somaB;
float filtroA;
float filtroB[20];
float altitude;
float apojas = 0;
float temperatura;
int pressao;
int n = 0;
int j = 0;
int r = 0;
int y = 1;
int apogeu = 0;
int counter = 0;
int estado1 = 0;
int estado2 = 0;
int estado3 = 0;
String nome = "calvo";
String nomearq = "";
unsigned long tempo1 = 0;
unsigned long tempo2 = 0;
unsigned long tempo3 = 0;

void setup() {
  Serial.begin(115200);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  // erro de iniciar os sensores

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  while (!Serial) { ; }
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1)
      ;
  }
  Serial.println("card initialized.");

  String dataString = "";
  dataString += String("Temperatura\tpressão\tAltitude\tfiltro1\tfiltro2\tN\tapogeu\tparaquedas1\tempo\t") + "\t";
  
  Serial.println(dataString);
  
  do {   
   String zeros = "";
    for(int tamanho = nome.length()+ String(counter).length(); tamanho < 8; tamanho++){
      zeros += "0";
    }
    
    nomearq = nome + zeros + String(counter) + ".txt";
    
    counter += 1;
  }
  while(SD.exists(nomearq));
  
  Serial.println(nomearq+"\t");
  
  File dataFile = SD.open(nomearq, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
  // sensores
  for (int i = 0; i < 20; i++) {

    alturainicial = alturainicial + bmp.readAltitude();
  }

  altura = (alturainicial / 20);
}

void loop() {
  unsigned long atualMillis = millis();

  altitude = bmp.readAltitude() - altura;
  height[j] = altitude;
  j++;

  if (j >= 20) {
    j = 0;
  }

  somaH = 0;

  for (int k = 0; k < 20; k++) {
    somaH = somaH + height[k];
  }
  filtroA = somaH / 20;

  filtroB[r] = filtroA;
  r++;

  if (r >= 20) {
    r = 0;
  }

  somaB = 0;
  for (int v = 0; v < 20; v++) {
    somaB = somaB + filtroB[v];
  }
  float filtro = somaB / 20;


  if (apojas > filtro) {
    n = n + 1;
  } else {
    n = 0;
  }

  if (n >= 20) {
    apogeu = 1;
  }
  // acionamento paraquedas 1
  if (apogeu == 1 && estado1 == 0){
    digitalWrite(IGN_1, HIGH);
    estado1 = 1;
    tempo1 = atualMillis + 5000;
  }
  // acionamento do timer para o paraquedas 2
  if(apogeu == 1 && estado2 == 0){
    tempo2 = atualMillis + 8000;
    estado2 = 1;
  }
  // acionamento paraquedas 3
  if(apogeu == 1 && estado3 == 0 && filtro <= -3){
    digitalWrite(IGN_3, HIGH);
    estado3 = 1;
    tempo3 = atualMillis + 5000;
  }

  // desligamento do paraquedas 1  
  if (estado1 == 1 && atualMillis >= tempo1){
    digitalWrite(IGN_1, LOW);
    estado1 = 2;
  }
  // acionamento do paraquedas 2
  if(estado2 == 1 && atualMillis >= tempo2){
    digitalWrite(IGN_2, HIGH);
    estado2 = 2;
    tempo2 = atualMillis+5000;
  }
  // desligamento do paraquedas 2
  if(estado2 == 2 && atualMillis >= tempo2){
    digitalWrite(IGN_2, LOW);
    estado2 = 3;
  }
  // desligamento do paraquedas 3
  if(estado3 == 1 && atualMillis >= tempo3){
    digitalWrite(IGN_3, LOW);
    estado3 = 2;
  }


  apojas = filtro;
  temperatura = bmp.readTemperature();
  pressao = bmp.readPressure();
  
  File dataFile = SD.open(nomearq, FILE_WRITE);
  String dataString = "";

  dataString += String(temperatura) + "\t";
  dataString += String(pressao) + "\t";
  dataString += String(altitude) + "\t";
  dataString += String(filtroA) + "\t";
  dataString += String(filtro) + "\t";
  dataString += String(n/10.0) + "\t";
  dataString += String(apogeu) + "\t";
  dataString += String(estado1) + "\t";
  dataString += String(estado2) + "\t";
  dataString += String(estado3) + "\t";
  Serial.println(dataString);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
}
