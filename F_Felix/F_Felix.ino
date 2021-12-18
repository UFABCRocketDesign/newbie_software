#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h> //acelerometro
#include <Adafruit_HMC5883_U.h> //magnetometro
#include <L3G.h> //giroscopio

Adafruit_BMP085 bmp;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
L3G gyro;

#define chipSelect 53
#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/
#define tempoLigado 2500
#define atraso 0
#define qtdFiltros 2
#define qtdValores 10

#define use_relogio 1
#define use_sd 1

#define use_gyro 0
#define use_acc 0
#define use_mag 0
#define use_baro 1

#define use_gyro_x (use_gyro && 1)
#define use_gyro_y (use_gyro && 1)
#define use_gyro_z (use_gyro && 1)

#define use_acc_x (use_acc && 1)
#define use_acc_y (use_acc && 1)
#define use_acc_z (use_acc && 1)

#define use_mag_x (use_mag && 1)
#define use_mag_y (use_mag && 1)
#define use_mag_z (use_mag && 1)

#define use_altura (use_baro && 1)
#define use_pressao (use_baro && 0)
#define use_temp (use_baro && 0)
#define use_apogeu (use_altura && 1)
#define use_paraquedas1 (use_apogeu && 1)
#define use_paraquedas2 (use_apogeu && 1)
#define use_paraquedas3 (use_apogeu && 1)

#if use_altura
float media = 0;
float matriz[qtdFiltros][qtdValores]; // acrescentar
#endif
#if use_sd
String arquivo = "";
#endif
#if use_paraquedas1
int led = LOW;
int led2 = LOW;
unsigned long desligaLED = 0;
unsigned long desligaLED2 = 0;
unsigned long ligaLED2 = 0;
#endif
#if use_paraquedas2
int led3 = LOW;
int led4 = LOW;
unsigned long desligaLED3 = 0;
#endif
#if use_paraquedas3
int led5 = LOW;
int led6 = LOW;
#endif
#if use_apogeu
float apogeu = 0;
boolean detectaApogeu = false;
#endif

#if use_altura
float funcaoMediaMovel(float x, int y){ // ou entra media movel ou entra alt relativa
  float somaVet = 0;
  
  for (int i = 0; i < (qtdValores - 1); i++) {
    matriz[y][i] = matriz[y][i + 1];
  }
  
  matriz[y][qtdValores - 1] = x;

  for (int i = 0; i < qtdValores; i++) {
    somaVet += matriz[y][i];
  }

  float mediaMovel = somaVet / (float)qtdValores;

  return mediaMovel;
}
#endif

#if use_apogeu
boolean funcaoDetectaApogeu (float segundaMediaMovel){
  
  if (apogeu < segundaMediaMovel) {
     apogeu = segundaMediaMovel;
  }
  
  float diferenca = apogeu - segundaMediaMovel;
  
  if (diferenca >= 1) {
    if (detectaApogeu == false){
      detectaApogeu = true;
    }
  }
  
  return detectaApogeu;
}

void funcaoAtivaParaquedas1 (unsigned long tempoAtual){
  
   if (led == LOW){
      digitalWrite(IGN_1, HIGH);
      led2 = HIGH;
      led = HIGH;
      desligaLED = tempoAtual + tempoLigado;
    }
    
    else if (led2 == HIGH) {
      if (tempoAtual >= desligaLED) {
        digitalWrite(IGN_1, LOW);
        led2 = LOW;
      }
  }
}

void funcaoAtivaParaquedas2 (unsigned long tempoAtual){
  if (ligaLED2 == 0){
    ligaLED2 = tempoAtual + atraso;
  }

  Serial.println(ligaLED2);
  
  if (led3 == LOW && tempoAtual >= ligaLED2 ) {
      digitalWrite(IGN_2, HIGH);
      led4 = HIGH;
      led3 = HIGH;
      desligaLED2 = tempoAtual + tempoLigado;
    }

  else if (led4 == HIGH) {
     if (tempoAtual >= desligaLED2 ) {
        digitalWrite(IGN_2, LOW);
        led4 = LOW;
      }
    }
}

void funcaoAtivaParaquedas3 (float segundaMediaMovel, unsigned long tempoAtual){
  
  if (led5 == LOW && segundaMediaMovel <= 10 ) {
      digitalWrite(LED_BUILTIN , HIGH);
      led6 = HIGH;
      led5 = HIGH;
      desligaLED3 = tempoAtual + tempoLigado;
    }

    if (led6 == HIGH) {
      if (tempoAtual >= desligaLED3) {
        digitalWrite(LED_BUILTIN, LOW);
        led6 = LOW;
      }
    }
}

#endif
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);

  Serial.begin(115200);
  Wire.begin();

  #if use_acc
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  }

  accel.setRange(ADXL345_RANGE_16_G);
  #endif //accel

  #if use_mag
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
  }
  #endif //mag

  #if use_gyro
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
  }
  
  gyro.enableDefault();
  #endif //gyro

  #if use_baro
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  #endif

  Serial.print("Initializing SD card...");
  #if use_sd
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
  }
  #endif
  Serial.println("card initialized.");

  String dataString = "";

  #if use_relogio
  dataString += ("Relógio\t");
  #endif
  #if use_temp
  dataString += ("Temp.(*C)\t");
  #endif
  #if use_pressao
  dataString += ("Pres.(Pa)\t");
  #endif
  #if use_altura
  dataString += ("Variação\t");
  #endif
  #if use_altura
  dataString += ("Média\t");
  dataString += ("Média 2\t");
  #endif
  #if use_acc_x
  dataString += ("AceleromêtroX\t");
  #endif
  #if use_acc_y
  dataString += ("AceleromêtroY\t");
  #endif
  #if use_acc_z
  dataString += ("AceleromêtroZ\t");
  #endif
  #if use_mag_x
  dataString += ("MagnetromêtroX\t");
  #endif
  #if use_mag_y
  dataString += ("MagnetromêtroY\t");
  #endif
  #if use_mag_z
  dataString += ("MagnetromêtroZ\t");
  #endif
  #if use_gyro_x
  dataString += ("GiroscópioX\t");
  #endif
  #if use_gyro_y
  dataString += ("GiroscópioY\t");
  #endif
  #if use_gyro_z
  dataString += ("GiroscópioZ\t");
  #endif

  #if use_sd
  String nome = "f";
  int cont = 0;
  String aux = (String) cont;
  String txt = ".txt";
  bool existente = false;

  while (existente == false) {

    String aux = (String) cont;
    int tamanho = 8 - (aux.length() + nome.length());
    String zero = "";

    for ( int i = 0; i < tamanho; i++) {
      zero = zero + "0";
    }

    arquivo = nome + zero + aux + txt;

    if (SD.exists(arquivo)) {
      cont = cont + 1;
      existente = false;
    }
    else {
      File dataFile = SD.open(arquivo, FILE_WRITE);
      dataFile.println(dataString);
      dataFile.close();
      existente = true;
    }
  }
  #endif

  #if use_altura
  float soma = 0; 
  for (int j = 0; j < 100; j++) {
    float alt = bmp.readAltitude();
    soma += alt;
  }

  media = soma / 100.0;
  #endif
  
  Serial.println(dataString);
}

// the loop function runs over and over again forever
void loop() {

  #if use_acc
  sensors_event_t eventAcc; 
  accel.getEvent(&eventAcc);
  #endif

  #if use_mag
  sensors_event_t eventMag;
  mag.getEvent(&eventMag);
  #endif

  #if use_gyro
  gyro.read();
  #endif

  #if use_sd
  String dataString = "";
  #endif

  #if use_relogio
  unsigned long tempoAtual = millis();

  dataString += String(tempoAtual/1000.0);
  dataString += ("\t");
  #endif

  #if use_altura
  float mediaMovel = 0;
  float segundaMediaMovel = 0;
  float alt = bmp.readAltitude();

  float altRelativa = alt - media;

  mediaMovel = funcaoMediaMovel(altRelativa, 0);

  segundaMediaMovel = funcaoMediaMovel(mediaMovel,1);
  
  #endif

  #if use_temp
  dataString += String(bmp.readTemperature());
  dataString += ("\t");
  #endif

  #if use_pressao
  dataString += String(bmp.readPressure());
  dataString += ("\t");
  #endif

  #if use_altura
  dataString += String(altRelativa);
  dataString += ("\t");

  dataString += String(mediaMovel);
  dataString += ("\t");

  dataString += String(segundaMediaMovel);
  dataString += ("\t");
  #endif

  #if use_acc_x
  dataString += String(eventAcc.acceleration.x);
  dataString += ("\t");
  #endif
  #if use_acc_y
  dataString += String(eventAcc.acceleration.y);
  dataString += ("\t");
  #endif
  #if use_acc_z
  dataString += String(eventAcc.acceleration.z);
  dataString += ("\t");
  #endif
  
  #if use_mag_x
  dataString += String(eventMag.magnetic.x);
  dataString += ("\t");
  #endif
  #if use_mag_y
  dataString += String(eventMag.magnetic.y);
  dataString += ("\t");
  #endif
  #if use_mag_z
  dataString += String(eventMag.magnetic.z);
  dataString += ("\t");
  #endif
  
  #if use_gyro_x
  dataString += String((int)gyro.g.x);
  dataString += ("\t");
  #endif
  #if use_gyro_y
  dataString += String((int)gyro.g.y);
  dataString += ("\t");
  #endif
  #if use_gyro_z
  dataString += String((int)gyro.g.z);
  dataString += ("\t");
  #endif

  #if use_apogeu
  detectaApogeu = funcaoDetectaApogeu(segundaMediaMovel);   
    
  if (detectaApogeu == true) {
    dataString += String("caindo\t");

  #if use_paraquedas1
    funcaoAtivaParaquedas1(tempoAtual);
  #endif
   
   #if use_paraquedas2
     funcaoAtivaParaquedas2 (tempoAtual);
   #endif
 
    #if use_paraquedas3
     funcaoAtivaParaquedas3 (segundaMediaMovel, tempoAtual);
    #endif
  }
  #endif

  #if use_sd
  File dataFile = SD.open(arquivo, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening" + arquivo);
  }
  #endif
  
  Serial.println(dataString);

 // delay(100);
}
