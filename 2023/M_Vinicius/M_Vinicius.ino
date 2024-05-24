// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE)
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
//Acelerometro
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
// Giroscopio
#include <L3G.h>
L3G gyro;
// Magnetometro
#include <Adafruit_HMC5883_U.h>
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
Adafruit_BMP085 bmp;
#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif  // ARDUINO_AVR_MEGA2560
#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/
int paraquedas = LOW;
unsigned long previousMillis = 0;
int paraquedas2 = LOW;
unsigned long previousMillis2 = 0;
bool verificar = false;
int paraquedas3 = LOW;
unsigned long previousMillis3 = 0;
bool verificar2 = false;
int paraquedas4 = LOW;
unsigned long previousMillis4 = 0;
bool verificar3 = false;
int i;
float altura, alt_in = 0;  // fazer o sensor pro foguete cair, 1 --> ta caindo
float altura_semRuido = 0;
float altura_sRuido2 = 0;
float vetor[10];
float filtro[10];
float filtro2[10];
float queda;
int index = 0;
int indi = 0;
float total = 0;
float acum = 0;
float apogeu[4];
int ap = 0;
int indicador = 0;


String marcos;
String marcs = "marcs";
String nome_do_arquivo;
const int chipSelect = 53;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
  Serial.begin(115200);
   Wire.begin();
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");
  // iniciar acelerometro
  if(!accel.begin())
    {
      /* There was a problem detecting the ADXL345 ... check your connections */
      Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
   }
  //iniciar giroscopio
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
  }
  gyro.enableDefault();
  // iniciar magnetometro
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
  
  }
  
  // CRIAR UM NOVO ARQUIVO DE TEXTO CADA VEZ QUE O CARTÃO SD É INSERIDO //
  do {
    String qnt_zero;
    for (i = String(indicador).length() + String(marcs).length(); i < 8; i++) {
      qnt_zero += "0";
    }
    nome_do_arquivo = marcs + qnt_zero + String(indicador) + ".txt";
    indicador++;
  } while (SD.exists(nome_do_arquivo));
  Serial.println(nome_do_arquivo);
  // ADICIONAR O CABEÇALHO //

  String cabString = "";

  cabString += ("Tempo (s)");
  cabString += "\t";

  cabString += ("Temperature (*C)");
  cabString += "\t";
  cabString += ("Pressure (Pa)");
  cabString += "\t";
  cabString += ("Altitude (meters)");
  cabString += "\t";
  cabString += ("Altitude sem ruido (meters)");
  cabString += "\t";
  cabString += ("Altitude s. ruido 2 (meters)");
  cabString += "\t";
  cabString += ("Detector de queda");
  cabString += "\t";
  cabString += ("Estado paraquedas");
  cabString += "\t";
  cabString += ("Estado paraquedas 2");
  cabString += "\t";
  cabString += ("Estado paraquedas 3");
  cabString += "\t";
   cabString += ("Estado paraquedas 4");
  cabString += "\t";
  cabString += ("Mag_X (uT)");
  cabString += "\t";
  cabString += ("Mag_Y");
  cabString += "\t";
  cabString += ("Mag_Z");
  cabString += "\t";
  cabString += ("Ace_X (m/s^2)");
  cabString += "\t";
  cabString += ("Ace_Y");
  cabString += "\t";
  cabString += ("Ace_Z");
  cabString += "\t";
  cabString += ("Giro_X");
  cabString += "\t";
  cabString += ("Giro_Y");
  cabString += "\t";
  cabString += ("Giro_Z");
  cabString += "\t";
  
  File cabFile = SD.open(nome_do_arquivo, FILE_WRITE);
  // if the file is available, write to it:
  if (cabFile) {
    cabFile.println(cabString);
    cabFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening" + nome_do_arquivo);
  }
  Serial.println(cabString);
  
if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (i = 0; i < 5; i++) {
    alt_in = alt_in + bmp.readAltitude();
  }
  alt_in = alt_in / 5;
}


void loop() {
 // Criação do dataString para armazenar as variaveis //
  String dataString = "";

unsigned long Tempo_decorrido = millis();

dataString += String(Tempo_decorrido/1000.0);
dataString += "\t"; 

  altura = bmp.readAltitude() - alt_in;

  // filtro 1 //
  filtro[index] = altura;
  index = (index + 1) % 10;
  total = 0;
  for (int i = 0; i < 10; i++) {
    total += filtro[i];
  }
  altura_semRuido = total / 10;
  // filtro 2 //
  filtro2[indi] = altura_semRuido;
  indi = (indi + 1) % 10;
  acum = 0;
  for (int i = 0; i < 10; i++) {
    acum += filtro2[i];
  }

  altura_sRuido2 = acum / 10;

  // Criação do dataString para armazenar as variaveis //
  


  dataString += bmp.readTemperature();
  dataString += "\t";
  dataString += bmp.readPressure();
  dataString += "\t";
  dataString += String(altura);
  dataString += "\t";
  dataString += String(altura_semRuido);
  dataString += "\t";
  dataString += String(altura_sRuido2);
  dataString += "\t";
 
  // DETECTAR APOGEU //
  for (i = 3; i > 0; i--) {
    apogeu[i] = apogeu[i - 1];
  }
  apogeu[0] = altura_sRuido2;
  if (apogeu[0] < apogeu[1] && apogeu[1] < apogeu[2] && apogeu[2] < apogeu[3]) {
    queda = 1;  //caindo
  } else {
    queda = 0;
  }
  dataString += String(queda);
  dataString += "\t";
  // LIBERAR O PRIMEIRO PARAQUEDAS //
  unsigned long currentMillis = millis();
  if (queda == 1) {
    if (previousMillis == 0 && paraquedas == LOW) {
      paraquedas = HIGH;  // ligado
      previousMillis = currentMillis;
        
    } else if (currentMillis - previousMillis > 4000) {
      paraquedas = LOW;  //desligado
    }
    // LIBERAR O SEGUNDO PARAQUEDAS //
    if (queda == 1 && previousMillis2 == 0) {
      previousMillis2 = currentMillis;
        
    }
    if (currentMillis - previousMillis2 >= 10000 && paraquedas2 == LOW && verificar == false) {
      paraquedas2 = HIGH;  // ligado
      previousMillis2 = currentMillis;
    } else if (currentMillis - previousMillis2 > 11000) {
      paraquedas2 = LOW;  //desligado
      verificar = true;
    }
    // LIBERAR O TERCEIRO PARAQUEDAS //
    if (altura_sRuido2 <= -5 && verificar2 == false) {
      paraquedas3 = HIGH;  // ligado
      previousMillis3 = currentMillis;
      verificar2 = true;
      previousMillis4 = currentMillis + 5000;
    } else if (currentMillis - previousMillis3 > 3000) {
      paraquedas3 = LOW;  //desligado
      
    }
    // LIBERAR O QUARTO PARAQUEDAS //
    if ( paraquedas4 == LOW && verificar2 == true && verificar3 == false && currentMillis >= previousMillis4 ) {
    paraquedas4 = HIGH; // ligado
    previousMillis4 = currentMillis; 
    verificar3 = true;                              
    }  
    else if (currentMillis - previousMillis4> 3000) {
    paraquedas4 = LOW;  //desligado  
        
    }
    digitalWrite(IGN_1, paraquedas);
    digitalWrite(IGN_2, paraquedas2);
    digitalWrite(IGN_3, paraquedas3);
    digitalWrite(IGN_4,paraquedas4);
  }
  dataString += String(paraquedas);
  dataString += "\t";
  dataString += String(paraquedas2);
  dataString += "\t";
  dataString += String(paraquedas3);
  dataString += "\t";
  dataString += String(paraquedas4);
  dataString += "\t";
  // Magnetometro & Acelerometro e Giroscopio
  //mag
  sensors_event_t Mag_event; 
  mag.getEvent(&Mag_event);
   dataString +=(Mag_event.magnetic.x); 
   dataString += "\t";
   dataString +=(Mag_event.magnetic.y);  
   dataString += "\t";
   dataString +=(Mag_event.magnetic.z);  
   dataString += "\t";
  
  //acelerometro
  sensors_event_t Ace_event; 
  accel.getEvent(&Ace_event);
   dataString +=(Ace_event.acceleration.x); 
   dataString += "\t";
   dataString +=(Ace_event.acceleration.y);  
   dataString += "\t";
   dataString +=(Ace_event.acceleration.z);  
   dataString += "\t";
  
  //giro
  gyro.read();
  dataString +=((int)gyro.g.x);
  dataString += "\t";
  dataString +=((int)gyro.g.y);
  dataString += "\t";
  dataString +=((int)gyro.g.z);
  dataString += "\t";
  // SD CARD //
  File dataFile = SD.open(nome_do_arquivo, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening" + nome_do_arquivo);
  }
  Serial.println(dataString);
}