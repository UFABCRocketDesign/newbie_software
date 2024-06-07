// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE) & Acelerometro & Giroscopio & Magnetometro
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>

#define SDD 1
#define BMP 1
#define BMP_ALT (BMP && 1)
#define BMP_TEMP (BMP && 1)
#define BMP_PRES (BMP && 1)

#define ACEL 1
#define ACEL_X (ACEL && 1)
#define ACEL_Y (ACEL && 1)
#define ACEL_Z (ACEL && 1)

#define GIRO 1
#define GIRO_X (GIRO && 1)
#define GIRO_Y (GIRO && 1)
#define GIRO_Z (GIRO && 1)

#define MAG 1
#define MAG_X (MAG && 1)
#define MAG_Y (MAG && 1)
#define MAG_Z (MAG && 1)

#define PARAQUEDAS (BMP && 1)
// PINOS PARAQUEDAS
#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/
int IGN[] = {IGN_1, IGN_2, IGN_3, IGN_4};

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif  // ARDUINO_AVR_MEGA2560

#if BMP
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#endif

#if SDD
#include <SPI.h>
#include <SD.h>
#endif

#if ACEL
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
#endif

#if GIRO
#include <L3G.h>
L3G gyro;
#endif

#if MAG
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
#endif

// Declaração variaveis paraquedas:
#if PARAQUEDAS
//int paraquedas = LOW;
//unsigned long previousMillis = 0;
int paraquedas2 = LOW;
//unsigned long previousMillis2 = 0;
//bool verificar = false;
int paraquedas3 = LOW;
//unsigned long previousMillis3 = 0;
bool verificar2 = false;
bool verificar2_2 = false;
int paraquedas4 = LOW;
//unsigned long previousMillis4 = 0;
bool verificar3 = false;
bool verificar4 = false;

#endif

// Declaração De variaveis diversas
bool queda;
float alt_in = 0;  // fazer o sensor pro foguete cair, 1 --> ta caindo
float apogeu[4];
float filtro[2][10];
int index[2];
bool paraquedas[4];
unsigned long previousMillis[4];
bool verificar[4];

String marcs = "marcs";
String nome_do_arquivo;

const int chipSelect = 53;


// FUNÇÕES

float filtro_altura(float altura, int qual) {  //FILTRO ALTURA

  filtro[qual][index[qual]] = altura;
  index[qual] = (index[qual] + 1) % 10;
  float total = 0;
  for (int i = 0; i < 10; i++) {
    total += filtro[qual][i];
  }
  float altura_semRuido = total / 10;
  return altura_semRuido;
}

bool det_apogeu(float altura) {  // DETECÇÃO DE APOGEU
  for (int i = 3; i > 0; i--) {
    apogeu[i] = apogeu[i - 1];
  }
  apogeu[0] = altura;
  if (apogeu[0] < apogeu[1] && apogeu[1] < apogeu[2] && apogeu[2] < apogeu[3]) {
    //queda = 1;  //caindo
    return true;
  } else {
    return false;
  }
}


// PARAQUEDAS
void acionar_paraquedas(bool queda, int pino, int qual, unsigned long currentMillis, float altura_sRuido2) {
  if (queda) {
    if (altura_sRuido2 > -5) {
      if (qual == 0 || previousMillis[qual] + 5000 < currentMillis && paraquedas[qual] == LOW && verificar[qual] == false) {
        paraquedas[qual] = HIGH;  // ligado
        previousMillis[qual] = currentMillis;

        if (qual == 0) {
          previousMillis[qual + 1] = previousMillis[qual];
        }

      } else if (currentMillis - previousMillis[qual] > 4000 && paraquedas[qual] == HIGH) {
        paraquedas[qual] = LOW;  //desligado
        verificar[qual] = true;
      }
    }

    else if (qual == 2 || previousMillis[qual] + 5000 < currentMillis && paraquedas[qual] == LOW && verificar[qual] == false) {
      paraquedas[qual] = HIGH;
      previousMillis[qual] = currentMillis;

      if (qual == 2) {
        previousMillis[qual + 1] = currentMillis;
      }

    } else if (currentMillis - previousMillis[qual] > 4000) {
      paraquedas[qual] = LOW;
      verificar[qual] = true;
    }

    digitalWrite(pino, paraquedas[qual]);
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  Serial.print("Initializing SD card...");


#if ACEL
  // iniciar ACELEROMETRO
  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  }
#endif

#if GIRO
  //iniciar GIROSCOPIO
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
  }
  gyro.enableDefault();

#endif

#if MAG
  // iniciar MAGNETOMETRO
  if (!mag.begin()) {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
  }

#endif

  // ADICIONAR O CABEÇALHO //

  String cabString = "";

  cabString += ("Tempo (s)");
  cabString += "\t";
#if BMP_TEMP
  cabString += ("Temperature (*C)");
  cabString += "\t";
#endif
#if BMP_PRES
  cabString += ("Pressure (Pa)");
  cabString += "\t";
#endif
#if BMP_ALT
  cabString += ("Altitude (meters)");
  cabString += "\t";
  cabString += ("Altitude sem ruido (meters)");
  cabString += "\t";
  cabString += ("Altitude s. ruido 2 (meters)");
  cabString += "\t";
  cabString += ("Detector de queda");
  cabString += "\t";
#endif
#if PARAQUEDAS
  cabString += ("Estado paraquedas");
  cabString += "\t";
  cabString += ("Estado paraquedas 2");
  cabString += "\t";
  cabString += ("Estado paraquedas 3");
  cabString += "\t";
  cabString += ("Estado paraquedas 4");
  cabString += "\t";
#endif
#if MAG_X
  cabString += ("Mag_X (uT)");
  cabString += "\t";
#endif
#if MAG_Y
  cabString += ("Mag_Y");
  cabString += "\t";
#endif
#if MAG_Z
  cabString += ("Mag_Z");
  cabString += "\t";
#endif
#if ACEL_X
  cabString += ("Ace_X (m/s^2)");
  cabString += "\t";
#endif
#if ACEL_Y
  cabString += ("Ace_Y");
  cabString += "\t";
#endif
#if ACEL_Z
  cabString += ("Ace_Z");
  cabString += "\t";
#endif
#if GIRO_X
  cabString += ("Giro_X");
  cabString += "\t";
#endif
#if GIRO_Y
  cabString += ("Giro_Y");
  cabString += "\t";
#endif
#if GIRO_Z
  cabString += ("Giro_Z");
  cabString += "\t";
#endif

  //SD

#if SDD
  // VERIFICAR E INICIALIZAR
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");

  // CRIAR UM NOVO ARQUIVO DE TEXTO CADA VEZ QUE O CARTÃO SD É INSERIDO //
  int indicador = 0;  // NAO SEI SE ESSE INDICADOR VAI DA ERRADO
  do {
    String qnt_zero;
    for (int i = String(indicador).length() + String(marcs).length(); i < 8; i++) {
      qnt_zero += "0";
    }
    nome_do_arquivo = marcs + qnt_zero + String(indicador) + ".txt";
    indicador++;
  } while (SD.exists(nome_do_arquivo));
  Serial.println(nome_do_arquivo);
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
#endif

  Serial.println(cabString);

#if BMP
  //INICIAR BMP
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif

#if BMP_ALT
  //LEITURA ALTITUDE
  for (int i = 0; i < 5; i++) {
    alt_in = alt_in + bmp.readAltitude();
  }
  alt_in = alt_in / 5;

#endif
}


void loop() {

  // LEITURAS

  //tempo
  unsigned long currentMillis = millis();

// altura
#if BMP_ALT
  float altura = bmp.readAltitude() - alt_in;
#endif

//mag & acel & giro
#if MAG
  sensors_event_t Mag_event;
  mag.getEvent(&Mag_event);
#endif

#if ACEL
  sensors_event_t Ace_event;
  accel.getEvent(&Ace_event);
#endif

#if GIRO
  gyro.read();
#endif

  // PROCESSAMENTO

#if BMP_ALT
  // FILTRO ALTURA //     FUTURAMENTE POSSO ADD A IDEIA DE UM "for" PARA VARIOS FILTROS
  float altura_semRuido = filtro_altura(altura, 0);
  float altura_sRuido2 = filtro_altura(altura_semRuido, 1);


  // DETECTAR APOGEU //
  queda = queda || det_apogeu(altura_sRuido2);  // as duas barras (significam or), ou seja, se ja deu como queda verdadeiro uma vez, sempre mantera verdadeiro.


#endif
  

#if PARAQUEDAS

for (int i = 0 ; i < 4 ; i++){
  acionar_paraquedas(queda, IGN[i], i, currentMillis, altura_sRuido2);
  }

#endif


  // ARMAZENAMENTO DOS DADOS DataString //

  String dataString = "";

  dataString += String(currentMillis / 1000.0);
  dataString += "\t";

#if BMP_TEMP
  dataString += bmp.readTemperature();
  dataString += "\t";
#endif
#if BMP_PRES
  dataString += bmp.readPressure();
  dataString += "\t";
#endif
#if BMP_ALT
  dataString += String(altura);
  dataString += "\t";
  dataString += String(altura_semRuido);
  dataString += "\t";
  dataString += String(altura_sRuido2);
  dataString += "\t";
  dataString += String(queda);
  dataString += "\t";
#endif

//PARAQUEDAS
#if PARAQUEDAS
for (int i = 0; i<4; i++){
  dataString += String(paraquedas[i]);
  dataString += "\t";
}

#endif

//MAG
#if MAG_X
  dataString += (Mag_event.magnetic.x);
  dataString += "\t";
#endif
#if MAG_Y
  dataString += (Mag_event.magnetic.y);
  dataString += "\t";
#endif
#if MAG_Z
  dataString += (Mag_event.magnetic.z);
  dataString += "\t";
#endif

//ACEL
#if ACEL_X
  dataString += (Ace_event.acceleration.x);
  dataString += "\t";
#endif
#if ACEL_Y
  dataString += (Ace_event.acceleration.y);
  dataString += "\t";
#endif
#if ACEL_Z
  dataString += (Ace_event.acceleration.z);
  dataString += "\t";
#endif

//GIRO
#if GIRO_X
  dataString += ((int)gyro.g.x);
  dataString += "\t";
#endif
#if GIRO_Y
  dataString += ((int)gyro.g.y);
  dataString += "\t";
#endif
#if GIRO_Z
  dataString += ((int)gyro.g.z);
  dataString += "\t";
#endif


// SD CARD //
#if SDD
  File dataFile = SD.open(nome_do_arquivo, FILE_WRITE);
  // escrever o arquivo
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening" + nome_do_arquivo);
  }
#endif

  Serial.println(dataString);
}