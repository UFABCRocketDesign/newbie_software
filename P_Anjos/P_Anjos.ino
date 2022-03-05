//=========================== SOFTWARE DE VOO - ROCKET DESIGN ==========================================//
// Pedro Anjos

#include <Adafruit_BMP085.h>          //Biblioteca para o barômetro
#include <SPI.h>
#include <SD.h>                       //Biblioteca para o cartão SD
#include <Wire.h>                     //Biblioteca para o giroscópio
#include <L3G.h>                      //Biblioteca para o giroscópio

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#undef  IGN_3 46  /*act3*/
#define IGN_3 13
#define IGN_4 55  /*act4*/
#define InterDesligTimer 2000           //Intervalo de tempo para desligar o paraquedas A em segundos
#define InterA2 4000                   //Intervalo de tempo para ligar o 2º acionamento do paraquedas A
#define HParaquedasB 5                 //Altura de acionamento do paraquedas B em metros
#define dfaltura 2                     // Define o delta de altura que serve de critério para a determinação do apogeu


L3G gyro;
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
Adafruit_BMP085 bmp;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  while (!Serial) {
    ;                                 // wait for serial port to connect. Needed for native USB port only
  }
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
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
    c = 8 - (a + b);                                                                            // Guarda a quantidade de zeros necessária para se colocar entre "PA" e o nº da versão.
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
  Serial.println("Tempo\tApogeu(Hmax)\tAltura filtrada(H1)\tDelta\tAltura sensor\tTemperature(*C)\tPressure(Pa)\tPr.sealevel(calculated)(Pa)\tX\tY\tZ\tSituacao");//Cabecalho no acompanhamento
  File dataFile = SD.open(nome, FILE_WRITE);
  dataFile.println("Tempo\tApogeu(Hmax)\tAltura filtrada(H1)\tDelta\tAltura sensor\tTemperature(*C)\tPressure(Pa)\tPr.sealevel(calculated)(Pa)\tX\tY\tZ\tSituacao"); //Cabecalho no SD
  dataFile.close();

  for (int i = 0; i < 100; i++) {                                                               //Este for serve para definir a altitude da base de lancamento como valor de referencia.
    Soma = Soma + bmp.readAltitude();
  }
  AltitudeRef = Soma / 100;
}

// the loop function runs over and over again forever
void loop() {
  unsigned long currentMillis = millis();                                                     // Regsitra em que instante do tempo está
  gyro.read();                                                                                // Faz a leitura do sensor giroscópio
  String dataString = "";                                                                     // Serve para criar a string que vai guardar os dados para que eles sejam gravados no SD
  SomaMov = 0;
  MediaMov = bmp.readAltitude() - AltitudeRef;
  for (int j = 0; j < 3; j++) {
    for (int i = 8; i >= 0; i--) {                                                            // Laco apenas para a movimentação
      Vetor[j][i + 1] = Vetor[j][i];
    }
    Vetor[j][0] = MediaMov;
    SomaMov = 0;
    for (int i = 0; i < 10; i++) {                                                            // Laco para a somatoria dos valores
      SomaMov = SomaMov + Vetor[j][i];
    }
    MediaMov = SomaMov / 10;
  }
  H2 = H1;                                                                                    // Guardei a altitude de referencia (medicao anterior)
  H1 = MediaMov;                                                                              // Nova leitura de altitude

  if (Hmax < H1) {
    Hmax = H1;
  }
  Delta = Hmax - H1;

  dataString += String(currentMillis);
  dataString += "\t";
  dataString += String(Hmax);
  dataString += "\t";
  dataString += String(H1);
  dataString += "\t";
  dataString += String(Delta);
  dataString += "\t";
  dataString += String(Vetor[0][0]);
  dataString += "\t";
  dataString += String(bmp.readTemperature());
  dataString += "\t";
  dataString += String(bmp.readPressure());
  dataString += "\t";
  dataString += String(bmp.readSealevelPressure());
  dataString += "\t";
  dataString += String((int)gyro.g.x);
  dataString += "\t";
  dataString += String((int)gyro.g.y);
  dataString += "\t";
  dataString += String((int)gyro.g.z);
  dataString += "\t";

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
    // ========================================================= PARTE DO CODIGO QUE ESTAVA FUNCIONANDO ========================================================================
    //    if (((Delta >= dfaltura || Aceso == true) && Timer > 0) && Fim == true) { // Só excute esse if depois do Timer de 2s
    //      unsigned long currentMillis = millis();   //conta em que instante do tempo está
    //      if (currentMillis - previousMillis >= interval) {
    //        previousMillis = currentMillis;
    //        if (ledState == LOW) {
    //          ledState = HIGH;
    //          dataString += String("Descendo - Paraquedas On");
    //        } else {
    //          ledState = LOW;
    //          dataString += String("Descendo - Paraquedas Off");
    //          Fim = false;                // Finaliza a verificação do acionamento do paraquedas
    //        }
    //        digitalWrite(LED_BUILTIN, ledState);   // A partir do momento que a diferença de altitude for acima de 2, provavelmente o foguete está descendo. Acione o paraquedas
    //      }
    //    }
    // ==========================================================================================================================================================================
    dataString += String("Descendo");                                                            // Só imprime na tela para acompanhar o funcionamento do código
  } else {
    dataString += String("Subindo");                                                             // Só imprime na tela para acompanhar o funcionamento do código
  }

  File dataFile = SD.open(nome, FILE_WRITE);                                                      // Só curiosidade: este é o ponto que mais consome de processamento

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  } else {  // if the file isn't open, pop up an error:
    Serial.println("error opening P_ANJOS.txt");
  }
}
