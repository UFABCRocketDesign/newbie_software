// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE) & Acelerometro & Giroscopio & Magnetometro
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
//teste


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
int IGN[] = { IGN_1, IGN_2, IGN_3, IGN_4 };

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

int verificar[4];
bool paraquedas[4];
int alturaTarget[] = { 0, 0, -5, -5 };
long atraso[] = { 0, 2000, 0, 2000 };
#define tempoLigado 5000
long proxAcao[4];

#endif

// Declaração De variaveis diversas
bool queda = false;
float altIn = 0;  // fazer o sensor pro foguete cair, 1 --> ta caindo

#define LARGURA_APG 25
float apogeu[LARGURA_APG];
float filtro[2][10];
int index[2];
//unsigned long tempo_anterior[4];

String marcs = "marcs";
String nomeDoArquivo;

const int chipSelect = 53;


// FUNÇÕES

float filtroAltura(float altura, int qual) {  //FILTRO ALTURA

  filtro[qual][index[qual]] = altura;
  index[qual] = (index[qual] + 1) % 10;
  float total = 0;
  for (int i = 0; i < 10; i++) {
    total += filtro[qual][i];
  }
  float alturaSemRuido = total / 10;
  return alturaSemRuido;
}

bool detApogeu(float altura) {  // DETECÇÃO DE APOGEU
  for (int i = LARGURA_APG ; i > 0; i--) {
    apogeu[i] = apogeu[i - 1];
  }
  apogeu[0] = altura;
  bool ret = true;
  
  for(int i = 1; (i< LARGURA_APG) && ret; i++){
    ret = ret && (apogeu[i-1]<apogeu[i]);
  }
   
   return ret;

}


// PARAQUEDAS
void acionarParaquedas(bool queda, int qual, unsigned long TempoAtual, float alturaAtual) {

  if (queda && ((alturaTarget[qual] == 0) || ((alturaTarget[qual] != 0) && (alturaAtual <= alturaTarget[qual])))) {

    // criar variavel para o cas oda cond atendida   
      alturaTarget[qual] = 0;    
    
    if (verificar[qual] == 0) {
      proxAcao[qual] = TempoAtual + atraso[qual];
      verificar[qual] = 1;
    }

    // Atraso
    if (verificar[qual] == 1 && TempoAtual >= proxAcao[qual]) {
      paraquedas[qual] = HIGH;
      verificar[qual] = 2;
      proxAcao[qual] = TempoAtual + tempoLigado;
    }

    // Tempo Ligado
    if (verificar[qual] == 2 && TempoAtual >= proxAcao[qual]) {
      paraquedas[qual] = LOW;
      verificar[qual] = 3;
    }
  }

  digitalWrite(IGN[qual], paraquedas[qual]);
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
   nomeDoArquivo = marcs + qnt_zero + String(indicador) + ".txt";
    indicador++;
  } while (SD.exists(nomeDoArquivo));
  Serial.println(nomeDoArquivo);
  File cabFile = SD.open(nomeDoArquivo, FILE_WRITE);
  // if the file is available, write to it:
  if (cabFile) {
    cabFile.println(cabString);
    cabFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening" + nomeDoArquivo);
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
    altIn = altIn + bmp.readAltitude();
  }
  altIn = altIn / 5;

#endif
}


void loop() {

  // LEITURAS

  //tempo
  unsigned long currentMillis = millis();

// altura
#if BMP_ALT
  float altura = bmp.readAltitude() - altIn;
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
  float alturaSemRuido = filtroAltura(altura, 0);
  float alturaSRuido2 = filtroAltura(alturaSemRuido, 1);


  // DETECTAR APOGEU //
  queda = queda || detApogeu(alturaSRuido2);  // as duas barras (significam or), ou seja, se ja deu como queda verdadeiro uma vez, sempre mantera verdadeiro.


#endif


#if PARAQUEDAS

  for (int i = 0; i < 4; i++) {
    acionarParaquedas(queda, i, currentMillis, alturaSRuido2);
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
  dataString += String(alturaSemRuido);
  dataString += "\t";
  dataString += String(alturaSRuido2);
  dataString += "\t";
  dataString += String(queda);
  dataString += "\t";
#endif

//PARAQUEDAS
#if PARAQUEDAS
  for (int i = 0; i < 4; i++) {
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
  File dataFile = SD.open(nomeDoArquivo, FILE_WRITE);
  // escrever o arquivo
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening" + nomeDoArquivo);
  }
#endif

  Serial.println(dataString);
}