#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_ADXL345_U.h>
#include <L3G.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_BMP085 bmp;
L3G gyro;

#define use_gyro 0
#define use_mag 0
#define use_accel 0

#define l 20 // tamanho
#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/
#define h_paraquedas_2 10  // altura para acionar 2º paraquedas (led2)
#define ledPin LED_BUILTIN
#define interv_desliga_led 7000      // interval at which to blink (milliseconds)
#define interv_liga_led2 4000

float novaAlt=0.0;
float velhaAlt=0.0;
float media=0.0;
float h = 0.0;
float lista[l];
float lista2[l];
float media_mov = 0;
float media_mov2 = 0;
const int chipSelect = 53;
String cabecalho = "";
String nomeArquivo = "";
int encontra_apogeu=0;
int apogeu_detectado = false;
int laco_led_2 = false;      // variavel para entrar no laço liga led 2 
int laco_led_3 = false;      // variavel para entrar no laço liga led 3 (built in)
#if use_mag    // se for usar o magnetometro
float mag_X;
float mag_Y;
float mag_Z;
#endif
#if use_accel   // se for usar o acelerometro
float accel_X;
float accel_Y;
float accel_Z;
#endif
#if use_gyro   // se for usar o giroscopio
float gyro_X;
float gyro_Y;
float gyro_Z;
#endif

int ledState1 = LOW;    // ledState used to set the LED
int ledState2 = LOW; 
int ledState3 = LOW;
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long liga_led2 = 0;        // quando o led tem que acender
unsigned long desliga_led1 = 0; 
unsigned long desliga_led2 = 0; // quando o led tem que desligar após o apogeu
unsigned long desliga_led3 = 0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  // TESTANDO SE OS SENSORES/SD ESTÃO FUNCIONANDO
  
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  #if use_mag
    /* Initialise the mag sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
  }
  #endif  //mag

  #if use_accel
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  }
  #endif   //accel

  #if use_gyro
  /* gyro */
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
  }
  gyro.enableDefault();
  #endif   // gyro

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  Serial.println("card initialized."); 

  #if use_accel
  // ACCELEROMETER RANGE

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // accel.setRange(ADXL345_RANGE_8_G);
  // accel.setRange(ADXL345_RANGE_4_G);
  // accel.setRange(ADXL345_RANGE_2_G);
  #endif

  // MÉDIA DE ALTITUDE
  
  for (int i=0; i<20; i++) {
    media =  media + bmp.readAltitude();
  }
  media = media / 20;

  cabecalho += "Tempo [s]";
  cabecalho += "\tAltitude [m]\tAltura [m]\tFiltro1 (h)\tFiltro2 (h)";
  cabecalho += "\tTemperatura [*C]";
  cabecalho += "\tPressao [Pa]\t";   //Pressao no nivel do mar [Pa]";
  #if use_mag
  cabecalho += "\tMag eixo X [uT]\tMag eixo Y [uT]\tMag eixo Z [uT]";
  #endif
  #if use_accel
  cabecalho += "\tAccel eixo X [m/s^2]\tAccel eixo Y [m/s^2]\tAccel eixo Z [m/s^2]";
  #endif
  #if use_gyro
  cabecalho += "\tGyro eixo X\tGyro eixo Y\tGyro eixo Z";
  #endif
  cabecalho += "\tApogeu";

  // CRIANDO UM NOVO ARQUIVO PARA SALVAR NO SD
  
  String nome = "gabi"; 
  int tamNome = nome.length();
  int num = 0;
  String txt = ".txt";
  bool tmp = false;
  while (tmp == false) {
    String zeros = "";
    int tamNum = String(num).length();
    int tamTotal = tamNome + tamNum;
    for (int j=0; j < (8-tamTotal); j++) {
      zeros = zeros + "0";
    }
    nomeArquivo = nome + zeros + String(num) + txt;
    if (SD.exists(nomeArquivo)) {
      num = num + 1;
      tmp = false;
    }
    else {
      File dataFile = SD.open(nomeArquivo, FILE_WRITE);
      dataFile.println(cabecalho);
      dataFile.close();
      tmp = true;
    }
  }
  Serial.println("file: " + nomeArquivo);
  Serial.println(cabecalho);
}

void loop() {
  unsigned long t_atual = millis();
  float t_atual_segundos = t_atual/1000.0;
  // make a string for assembling the data to log:
  String dataString = "";

  //// LEITURA DE SENSORES
  
  // Calculate altitude assuming 'standard' barometric pressure of 1013.25 millibar = 101325 Pascal
  float novaAlt=bmp.readAltitude();
  h = novaAlt - media;  //altura 

  //temperatura
  float temp =  bmp.readTemperature(); 
   
  //pressão
  float pressao = bmp.readPressure(); 
     //pressão no nível do mar
     //float pressaoNivelMar = bmp.readSealevelPressure(); 

  #if use_mag
  // Magnetometro
  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);
  mag_X = event.magnetic.x;
  mag_Y = event.magnetic.y;
  mag_Z = event.magnetic.z;
  #endif
  
  #if use_accel
  // Acelerometro
  /* Get a new sensor event */ 
  //sensors_event_t event; 
  accel.getEvent(&event);
  accel_X = event.acceleration.x;
  accel_Y = event.acceleration.y;
  accel_Z = event.acceleration.z;
  #endif

  #if use_gyro
  // Giroscopio
  gyro.read();
  gyro_X = (int)gyro.g.x;
  gyro_Y = (int)gyro.g.y;
  gyro_Z = (int)gyro.g.z;
  #endif
  
  //// FILTROS DE ALTURA
  
  // filtro 1
  for (int k=0; k<(l-1); k++) {
    lista[k] = lista[k+1];
  }
  lista[l-1] = h;
  for (int j=0; j<l; j++) {
    media_mov = media_mov + lista[j];
  }
  media_mov = media_mov/l;
  
  // filtro 2
  for (int k=0; k<(l-1); k++) {
    lista2[k] = lista2[k+1];
  }
  lista2[l-1] = media_mov;
  for (int j=0; j<l; j++) {
    media_mov2 = media_mov2 + lista2[j];
  }
  media_mov2 = media_mov2/l;
      
  //// REUNINDO OS DADOS EM UMA STRING

  dataString += String(t_atual_segundos);
  dataString += "\t";
  dataString += String(novaAlt);
  dataString += "\t";
  dataString += String(h);
  dataString += "\t";
  dataString += String(media_mov);  // media_mov é o filtro 1 de altura
  dataString += "\t";
  dataString += String(media_mov2);  // media_mov2 é o filtro 2 de altura
  dataString += "\t";
  dataString += String(temp);
  dataString += "\t";
  dataString += String(pressao);
     //dataString += "\t";
     //dataString += String(pressaoNivelMar);
  #if use_mag
  dataString += "\t";  
  dataString += String(mag_X);
  dataString += "\t";
  dataString += String(mag_Y);
  dataString += "\t";
  dataString += String(mag_Z);
  #endif
  #if use_accel
  dataString += "\t";  
  dataString += String(accel_X);
  dataString += "\t";  
  dataString += String(accel_Y);
  dataString += "\t";  
  dataString += String(accel_Z);
  #endif
  #if use_gyro
  dataString += "\t";  
  dataString += String(gyro_X);
  dataString += "\t";  
  dataString += String(gyro_Y);
  dataString += "\t";  
  dataString += String(gyro_Z);
  #endif

  // Encontrando o apogeu 

  if (media_mov2 < velhaAlt) {
    encontra_apogeu += 1;
  }
  else {
    encontra_apogeu = 0;
  }

  if (encontra_apogeu == 5) { 
    dataString += "\t"; 
    dataString += "Apogeu Detectado! Led 1";
    if (apogeu_detectado == false) {
      // LED 1
      ledState1 = HIGH;
      desliga_led1 = t_atual + interv_desliga_led;
      liga_led2 = t_atual + interv_liga_led2;
      apogeu_detectado = true;
      laco_led_2 = true;
      laco_led_3 = true;
    }
  }
  // LED 2
  if (t_atual >= liga_led2 && laco_led_2 == true){
    ledState2 = HIGH;
    dataString += "\t";
    dataString += "Led 2!";
    desliga_led2 = t_atual + interv_desliga_led;
    laco_led_2 = false;
  }
  // LED 3
  if (media_mov2 <= h_paraquedas_2 && laco_led_3 == true) {
    ledState3 = HIGH;
    dataString += "\t";
    dataString += "Led 3!";
    desliga_led3 = t_atual + interv_desliga_led;
    laco_led_3 = false;
  }

  if (t_atual >= desliga_led1) {
    ledState1 = LOW;
  }
  if (t_atual >= desliga_led2) {
    ledState2 = LOW;
  }
  if (t_atual >= desliga_led3) {
    ledState3 = LOW;
  }
  
  digitalWrite(IGN_1, ledState1);
  digitalWrite(IGN_2, ledState2);
  digitalWrite(ledPin, ledState3);

  // Mostrando dataString no Serial
  Serial.println(dataString);

  // abrir o arquivo do SD e armazenar dados
  File dataFile = SD.open(nomeArquivo, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }

  // Reiniciando variáveis de velha altitude e filtros de altura
  velhaAlt = media_mov2;
  media_mov = 0;
  media_mov2 = 0;
}
