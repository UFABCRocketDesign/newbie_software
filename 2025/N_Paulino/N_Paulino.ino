
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alt_zero = 0;              

int ind_vetor1 = 0;
float vetor_num1 [10];
float valor_flt1 = 0;

int ind_vetor2 = 0;
float vetor_num2 [10];
float valor_flt2 = 0;

int ind_vetor3 = 0;
float vetor_num3 [10];
float valor_flt3 = 0;

int ind_vetor4 = 0;
float vetor_num4 [10];
float valor_flt4 = 0;

int queda = 0;
int contador_queda = 0;
float ultimo_valor_flt4 = 0;
const int LIMITE_QUEDAS_SEGUIDAS = 10;


const int chipSelect = 53;

#include <SPI.h>
#include <SD.h>


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {           
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) { }               
  }
  float soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += bmp.readAltitude();
                      
  }
  alt_zero = soma/10.0;

//-----------------------------------------------------------------------------
  while (!Serial);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }

  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Temperature\tPressure\tflt1\tflt2\tflt3\tflt4");
    dataFile.close();
    }
  }

void loop() {
  
  float real_alt = bmp.readAltitude() - alt_zero;
  
  vetor_num1 [ind_vetor1] = real_alt;
  ind_vetor1 ++;
  if (ind_vetor1 >= 10) ind_vetor1 = 0;
    
  float valor_vet1 = 0;
  for (int i = 0; i < 10 ; i++) {
   valor_vet1 += vetor_num1 [i];
  }
  valor_flt1 = valor_vet1 / 10;

// -------------------------------------------------------------------------

  vetor_num2 [ind_vetor2] = valor_flt1;
  ind_vetor2 ++;
  if (ind_vetor2 >= 10) ind_vetor2 = 0;

  float valor_vet2 = 0;
  for (int i = 0; i < 10 ; i++) {
   valor_vet2 += vetor_num2 [i]; 
  }
  valor_flt2 = valor_vet2 / 10;

//--------------------------------------------------------------------------

  vetor_num3 [ind_vetor3] = valor_flt2;
  ind_vetor3 ++;
  if (ind_vetor3 >= 10) ind_vetor3 = 0;

  float valor_vet3 = 0;
  for (int i = 0; i < 10; i++) {
    valor_vet3 += vetor_num3 [i];
  }
  valor_flt3 = valor_vet3 / 10;

//--------------------------------------------------------------------------

  vetor_num4 [ind_vetor4] = valor_flt3 ;
  ind_vetor4 ++;
  if (ind_vetor4 >=10) ind_vetor4 = 0;

  float valor_vet4 = 0;
  for (int i = 0; i < 10; i++) {
    valor_vet4 += vetor_num4 [i];
  }
  valor_flt4 = valor_vet4 / 10;

//--------------------------------------------------------------------------

  if (valor_flt4 < ultimo_valor_flt4) {
    contador_queda++;
  } else {
    contador_queda = 0;  
  }
  if (contador_queda >= LIMITE_QUEDAS_SEGUIDAS) {
    queda = 1;
  } else {
    queda = 0;
  }
  ultimo_valor_flt4 = valor_flt4;

//--------------------------------------------------------------------------

  String dataString = "";
  dataString += String(bmp.readTemperature());
  dataString += String("\t");
  dataString += String(bmp.readPressure());
  dataString += String("\t");
  dataString += String(valor_flt1);
  dataString += String("\t");
  dataString += String(valor_flt2);
  dataString += String("\t");
  dataString += String(valor_flt3);
  dataString += String("\t");
  dataString += String(valor_flt4);


   File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }
  else {
    Serial.println("error opening datalog.txt");
  }
  Serial.println(dataString);
}