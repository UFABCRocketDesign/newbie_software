//=========================== SOFTWARE DE VOO - ROCKET DESIGN ==========================================//
// Pedro Anjos


#include <SPI.h>
#include <SD.h>                       //Biblioteca para o cartão SD
#include <Wire.h>                     //Biblioteca para o giroscópio e acelerômetro
#include <L3G.h>                      //Biblioteca para o giroscópio
#include <Adafruit_Sensor.h>          //Biblioteca para os sensores
#include <Adafruit_BMP085.h>          //Biblioteca para o barômetro
#include <Adafruit_ADXL345_U.h>       //Biblioteca para o acelerômetro
#include <Adafruit_HMC5883_U.h>       //Biblioteca para o magnetômetro

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#undef  IGN_3 46  /*act3*/
#define IGN_3 13
#define IGN_4 55  /*act4*/
#define InterDesligTimer 2000             //Intervalo de tempo para desligar o paraquedas A em segundos
#define InterA2 4000                      //Intervalo de tempo para ligar o 2º acionamento do paraquedas A
#define HParaquedasB 5                    //Altura de acionamento do paraquedas B em metros
#define dfaltura 2                        //Define o delta de altura que serve de critério para a determinação do apogeu
#define Barometro 1                       //Definindo a presença ou não de um sensor barométrico no sistema
#define altitude    (1 && Barometro)      //Definindo a presença de dados de altitude
#define temperatura (1&& Barometro)       //Definindo a presença de dados de temperatura
#define pressao     (0 && Barometro)      //Definindo a presença de dados de pressão
#define giroscopio 1                      //Definindo a presença ou não de um sensor giroscópio no sistema
#define gyrox (1 && giroscopio)           //Definindo a presença de dados no eixo X do giroscópio
#define gyroy (0 && giroscopio)           //Definindo a presença de dados no eixo Y do giroscópio
#define gyroz (0 && giroscopio)           //Definindo a presença de dados no eixo Z do giroscópio
#define Acelerometro 1                    //Definindo a presença ou não de um sensor acelerômetro no sistema
#define Accx (0 && Acelerometro)
#define Accy (1 && Acelerometro)
#define Accz (0 && Acelerometro)
#define Magnetometro 1                    //Definindo a presença ou não de um sensor magnetômetro no sistema
#define Magx (0 && Magnetometro)
#define Magy (1 && Magnetometro)
#define Magz (1 && Magnetometro)
#define GravacaoSD 0                      //Definindo se irá gravar os dados no cartão SD
// ======================================================================================================================= //
#if Barometro
Adafruit_BMP085 bmp;
/* Assign a unique ID to this sensor at the same time */
#endif
#if Acelerometro
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
/* Assign a unique ID to this sensor at the same time */
#endif
#if Magnetometro
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
#endif
#if giroscopio
L3G gyro;
#endif
// =========================================== VARIAVEIS DECLARADAS ============================================================================ //
const int chipSelect = 53;            //Define o pino para o chipselect para gravar no cartão SD
float H1 = 0;                         // Variável global - Não é ressetada a cada loop. Armazena o dado.
float H2 = 0;
float Hmax = 0;
float Soma = 0;
float SomaMov = 0;
float AltitudeRef = 0;
float MediaMov = 0;
float MediaMA = 0;
float MediaMB = 0;
float Delta;                          //Diferença da altitude anterior com a nova que foi medida
float Vetor[3][10];                   //Vetor para guardar os últimos 10 valores para a média móvel
float FiltroA[10];                    //Segunda filtragem para a média móvel
float FiltroB[10];                    // terceira filtragem para a média móvel
float SomaFA = 0;
float SomaFB = 0;
float Aux = 0;
int cont = 0;
String dataCabecalho;
String nome;
String parteA;
String parteB;
int a;
int b;
int c;
float Timer = 0;                      // Guarda o tempo do timer
unsigned long previousMillis = 0;     // Guarda o momento de tempo para iniciar A e B
unsigned long previousMillis2 = 0;    // Guarda o momento para o timer para manter B ligado
const long interval = 2000;           // O intervalo de tempo que o LED deve ficar ligado em milesegundos
int Intervalo = 3000;                 //Intervalo de tempo do Timer antes do acionamento do paraquedas
bool Apogeu = false;
bool Tia = true;
int TA_Piloto = 0;                    //Intervalo de tempo para desligar o paraquedas A em segundos
int TA_PilotoBackup = 0;               //Intervalo de tempo para ligar o 2º acionamento paraquedas A em segundos
int TDA_PilotoBackup = 0;              //Intervalo de tempo que o 2º acionamento paraquedas A fica ligado em segundos
int TB_Main = 0;                      //Intervalo de tempo para desligar o paraquedas B em segundos
bool Aceso = false;                   // A variável booleana para verificar se o LED ta ligado
bool Fim = true;                      // A variável booleana para parar a verificação do paraquedas
bool ResetA2 = true;                  // A variável booleana para parar resetar o timer do A2
bool ResetB = true;                   // A variável booleana para parar resetar o timer do B
//============================================================================================================================================= //

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
#ifndef ESP8266
  while (!Serial) {
    ;                                 // wait for serial port to connect. Needed for native USB port only
  }
#endif
  // ============================= PARTE APENAS DO GIROSCOPIO ======================================= //
#if giroscopio
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
#endif
  // ============================= PARTE APENAS DO BAROMETRO ========================================= //
#if Barometro
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif
  // ========================== PARTE APENAS DO ACELEROMETRO ===================================== //
#if Acelerometro
  //Serial.println("Accelerometer Test"); Serial.println("");Informações validas apenas quando está realizando testes

  /* Initialise the sensor */
  if (!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // accel.setRange(ADXL345_RANGE_8_G);
  // accel.setRange(ADXL345_RANGE_4_G);
  // accel.setRange(ADXL345_RANGE_2_G);

  /* Display some basic information on this sensor */
  //displaySensorDetails();  //Reitirei do código pois não era interessante. Se precisar olhe na biblioteca original

  /* Display additional settings (outside the scope of sensor_t) */
  //displayDataRate(); //Reitirei do código pois não era interessante. Se precisar olhe na biblioteca original
  //displayRange(); //Reitirei do código pois não era interessante. Se precisar olhe na biblioteca original
#endif
  // =============================== PARTE APENAS DO MAGNETÔMETRO ========================================= //
#if Magnetometro
  //Serial.println("HMC5883 Magnetometer Test"); Serial.println(""); Informações validas apenas quando está realizando testes

  /* Initialise the sensor */
  if (!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1);
  }

  /* Display some basic information on this sensor */
  //displaySensorDetails(); //Reitirei do código pois não era interessante. Se precisar olhe na biblioteca original
#endif
  // ================================================================================================ //
#if GravacaoSD
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  do {
    nome = "PA";
    a = nome.length();
    parteB = String(cont);
    b = parteB.length();
    c = 8 - (a + b);                                                                          // Guarda a quantidade de zeros necessária para se colocar entre "PA" e o nº da versão.
    for (int i = 0; i < c; i++) {
      nome += "0";
    }
    nome += parteB;
    nome += ".txt";
    cont = cont + 1;
  }
  while (SD.exists(nome));
  Serial.print("O arquivo será gravado com nome ");
  Serial.println(nome);
  Serial.println("card initialized.");
#endif
  // ==================================== CABEÇALHO ============================================================================================================= //
  dataCabecalho += "Tempo\t";
#if altitude
  dataCabecalho += "Apogeu(Hmax)\tAltura filtrada(H1)\tDelta\tAltura sensor\t";
#endif
#if temperatura
  dataCabecalho += "Temperature(*C)\t";
#endif
#if pressao
  dataCabecalho += "Pressão(bar)\t";
#endif
#if gyrox
  dataCabecalho += "X\t";
#endif
#if gyroy
  dataCabecalho += "Y\t";
#endif
#if gyroz
  dataCabecalho += "Z\t";
#endif
#if Accx
  dataCabecalho += "X_ddot\t";
#endif
#if Accy
  dataCabecalho += "Y_ddot\t";
#endif
#if Accz
  dataCabecalho += "Z_ddot\t";
#endif
#if Magx
  dataCabecalho += "magX(uT)\t";
#endif
#if Magy
  dataCabecalho += "magY(uT)\t";
#endif
#if Magz
  dataCabecalho += "magZ(uT)\t";
#endif
  dataCabecalho += "Situacao";
  Serial.println(dataCabecalho);
#if GravacaoSD
  File dataFile = SD.open(nome, FILE_WRITE);
  dataFile.println(dataCabecalho);
  dataFile.close();
#endif
  // =========================================================================================================================================================== //
#if Barometro
  for (int i = 0; i < 100; i++) {                                                            //Este "for" serve para definir a altitude da base de lancamento como valor de referencia.
    Soma = Soma + bmp.readAltitude();
  }
  AltitudeRef = Soma / 100;
#endif
}
// the loop function runs over and over again forever
void loop() {
  unsigned long currentMillis = millis();                                                     // Regsitra em milisegundos em que instante do tempo está
#if giroscopio
  gyro.read();                                                                                // Faz a leitura do sensor giroscópio
#endif
  String dataString = "";                                                                     // Serve para criar a string que vai guardar os dados para que eles sejam gravados no SD
  dataString += String(currentMillis / 1000.0);                                                 // Registra o tempo em segundos
  dataString += "\t";
  // ========================= MÉDIA MÓVEL E DETECÇÃO DE APOGEU =============================== //
#if Barometro
#if altitude
  SomaMov = 0;
  MediaMov = bmp.readAltitude() - AltitudeRef;
  for (int j = 0; j < 3; j++) {
    for (int i = 8; i >= 0; i--) {                                                            // Laço apenas para a movimentação
      Vetor[j][i + 1] = Vetor[j][i];
    }
    Vetor[j][0] = MediaMov;
    SomaMov = 0;
    for (int i = 0; i < 10; i++) {                                                            // Laço para a somatória dos valores
      SomaMov = SomaMov + Vetor[j][i];
    }
    MediaMov = SomaMov / 10;
  }
  H2 = H1;                                                                                    // Guardei a altitude de referência (medicao anterior)
  H1 = MediaMov;                                                                              // Nova leitura de altitude

  if (Hmax < H1) {
    Hmax = H1;
  }
  Delta = Hmax - H1;
  // ============================= DADOS QUE VÃO PARA STRING ================================== //
  dataString += String(Hmax);
  dataString += "\t";
  dataString += String(H1);
  dataString += "\t";
  dataString += String(Delta);
  dataString += "\t";
  dataString += String(Vetor[0][0]);
  dataString += "\t";
#endif
#if temperatura
  dataString += String(bmp.readTemperature());
  dataString += "\t";
#endif
#if pressao
  dataString += String(bmp.readPressure());
  dataString += "\t";
#endif
#endif
#if giroscopio
#if gyrox
  dataString += String((int)gyro.g.x);
  dataString += "\t";
#endif
#if gyroy
  dataString += String((int)gyro.g.y);
  dataString += "\t";
#endif
#if gyroz
  dataString += String((int)gyro.g.z);
  dataString += "\t";
#endif
#endif
  // ================================= ETAPA DO ACELERÔMETRO ==================================== //
  /* Get a new sensor event */
  sensors_event_t event;
#if Acelerometro

  accel.getEvent(&event);
  /* Display the results (acceleration is measured in m/s^2) */
#if Accx
  dataString += String(event.acceleration.x); dataString += "\t";
#endif
#if Accy
  dataString += String(event.acceleration.y); dataString += "\t";
#endif
#if Accz
  dataString += String(event.acceleration.z); dataString += "\t";
#endif
#endif
  // ================================== ETAPA DO MAGNETÔMETRO ==================================== //
#if Magnetometro
  /* Get a new sensor event */
  mag.getEvent(&event);

  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
#if Magx
  dataString += String(event.magnetic.x); dataString += "\t";
#endif
#if Magy
  dataString += String(event.magnetic.y); dataString += "\t";
#endif
#if Magz
  dataString += String(event.magnetic.z); dataString += "\t";
#endif
  // O código permite reajustar a oritenção para o Norte. Neste código isso não é necessário.
  // Caso precise, olhe na biblioteca original
#endif
  // ======================== ETAPA PARA ACIONAMENTO DE PARAQUEDAS ============================== //
#if Barometro
  if (Delta > 0 || Apogeu == true) {                                                              // Só serve para imprimir na tela. Se a diferença da média móvel com o Hmáx é maior que 0, significa que pode estar descendo
    if (Delta >= dfaltura || Apogeu == true) {                                                    // Se a diferença for maior ou igual ao delta de ref. ou já tenha detectado o apogeu
      Apogeu = true;                                                                              // Imprime na tela: Encontrou o apogeu
      dataString += String("Encontrou o apogeu");
      if (Tia) {
        previousMillis = currentMillis;                                                           // Começa a considerar este momento para inciar os timers
        TA_Piloto = InterDesligTimer + previousMillis;                                            // Guarda o instante para desligar o paraquedas A
        TA_PilotoBackup = InterA2 + previousMillis;                                               // Guarda o instante para o segundo acionamento do Paraquedas A (segurança)
        Tia = false;
      }
      if (currentMillis >= TA_Piloto) {
        dataString += String("PA - Off");                                                         // Desliga o paraquedas A
        digitalWrite(IGN_1, LOW);
      } else {
        dataString += String("PA - On ");
        digitalWrite(IGN_1, HIGH);                                                                // Ligou o paraquedas A
      }
      if (TA_PilotoBackup <= currentMillis && (currentMillis < TDA_PilotoBackup || TDA_PilotoBackup == 0)) {
        dataString += String("PA2 - On  ");                                                       // Segundo acionamento paraquedas A
        if (ResetA2) {
          digitalWrite(IGN_2, HIGH);
          TDA_PilotoBackup = InterDesligTimer + currentMillis;                                    // Atualiza o instante para desligar o segundo acionamento do paraquedas
          ResetA2 = false;
        }
      } else {
        dataString += String("PA2 - Off ");                                                       // Desliga o segundo acionamento paraquedas A
        digitalWrite(IGN_2, LOW);
      }
      if (H1 <= HParaquedasB && (currentMillis < TB_Main || TB_Main == 0)) {
        dataString += String("PB - On  ");                                                        // Ligou o parquedas B
        if (ResetB) {
          digitalWrite(IGN_3, HIGH);
          TB_Main = InterDesligTimer + currentMillis;                                             // Atualiza o instante para desligar o paraquedas B
          ResetB = false;
        }
      } else {
        dataString += String("PB - Off  ");                                                       // Desliga paraquedas B
        digitalWrite(IGN_3, LOW);
      }
    }
    dataString += String("Descendo");                                                            // Só imprime na tela para acompanhar o funcionamento do código
  } else {
    dataString += String("Subindo");                                                             // Só imprime na tela para acompanhar o funcionamento do código
  }
#endif

  // ======================== ETAPA PARA GRAVAR NO CARTÃO SD ===================================== //
#if GravacaoSD
  File dataFile = SD.open(nome, FILE_WRITE);                                                     // Só curiosidade: este é o ponto que mais consome de processamento

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  } else {  // if the file isn't open, pop up an error:
    Serial.println("error opening P_ANJOS.txt");
  }
#endif
}
