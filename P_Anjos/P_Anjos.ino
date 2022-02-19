// Código para detecção do apogeu de um foguete que utiliza sensor de pressão. Inclui gravação de dados no cartão SD e acionamento de paraquedas
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/



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
int ledState1 = LOW;                  // Estado do LED 1 que indica acionamento do paraquedas A
int ledState2 = LOW;                  // Outra LED para verificar o timer antes de acionar o paraquedas
float Timer = 0;                      // Guarda o tempo do timer
unsigned long previousMillis = 0;     // Guarda o momento de tempo para iniciar A e B
unsigned long previousMillis2 = 0;    // Guarda o momento para o timer para manter B ligado
const long interval = 2000;           // O intervalo de tempo que o LED deve ficar ligado em milesegundos
int Intervalo = 3000;                 //Intervalo de tempo do Timer antes do acionamento do paraquedas
bool Apogeu = false;
bool Tia = true;
int InterA = 1000;                    //Intervalo de tempo para desligar o paraquedas A em segundos
int InterB = 2000;                    //Intervalo de tempo para ligar o paraquedas B em segundos
int InterB2 = 1000;                   //Intervalo de tempo que o paraquedas B fica ligado em segundos
bool Aceso = false;                   // A variável booleana para verificar se o LED ta ligado
bool Fim = true;                      // A variável booleana para parar a verificação do paraquedas
float dfaltura = 2;                   // Define o delta de altura que serve de critério para a determinação do apogeu
Adafruit_BMP085 bmp;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
    ;                                 // wait for serial port to connect. Needed for native USB port only
  }
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
  Serial.println("Apogeu(Hmax)\tAltura filtrada final(H1)\tDelta\tAltura medida no sensor\tTemperature(*C)\tPressure(Pa)\tPressure at sealevel(calculated)(Pa)\tSituacao");//Cabecalho no acompanhamento
  File dataFile = SD.open(nome, FILE_WRITE);
  dataFile.println("Apogeu(Hmax)\tAltura filtrada final(H1)\tDelta\tAltura medida no sensor\tTemperature(*C)\tPressure(Pa)\tPressure at sealevel(calculated)(Pa)\tSituacao"); //Cabecalho no SD
  dataFile.close();

  for (int i = 0; i < 100; i++) {                                                               //Este for serve para definir a altitude da base de lancamento como valor de referencia.
    Soma = Soma + bmp.readAltitude();
  }
  AltitudeRef = Soma / 100;
}

// the loop function runs over and over again forever
void loop() {
  String dataString = "";                                                                       // Serve para criar a string que vai guardar os dados para que eles sejam gravados no SD
  SomaMov = 0;
  MediaMov = bmp.readAltitude() - AltitudeRef;
  for (int j = 0; j < 3; j++) {
    for (int i = 8; i >= 0; i--) {                                                              // Laco apenas para a movimentação
      Vetor[j][i + 1] = Vetor[j][i];
    }
    Vetor[j][0] = MediaMov;
    SomaMov = 0;
    for (int i = 0; i < 10; i++) {                                                              // Laco para a somatoria dos valores
      SomaMov = SomaMov + Vetor[j][i];
    }
    MediaMov = SomaMov / 10;
  }
  H2 = H1;                                                                                      // Guardei a altitude de referencia (medicao anterior)
  H1 = MediaMov;                                                                                // Nova leitura de altitude

  if (Hmax < H1) {
    Hmax = H1;
  }
  Delta = Hmax - H1;
  //  if (ledState1 == HIGH) {
  //    Aceso = true;                                                                               // Para garantir que após o acionamento do paraquedas, ele irá executar o próximo if
  //  }
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

  if (Delta > 0 || Apogeu == true) {                                                              // Só serve para imprimir na tela. Se a diferença da média móvel com o Hmáx é maior que 0, significa que pode estar descendo
    if (Delta >= dfaltura || Apogeu == true) {                                                    // Se a diferença for maior ou igual ao delta de ref. ou já tenha detectado o apogeu
      Apogeu = true;                                                                              // Imprime na tela: Encontrou o apogeu
      dataString += String("Encontrou o apogeu");
      unsigned long currentMillis = millis();                                                     // Regsitra em que instante do tempo está
      if (Tia = true) {
        previousMillis = currentMillis;                                                           // Começa a considerar este momento para inciar os timers
        InterA = InterA + previousMillis;                                                         // Guarda o instante para desligar o paraquedas A
        InterB = InterB + previousMillis;                                                         // Guarda o instante para ligar o paraquedas B
        InterB2 = InterB2 + InterB;                                                               // Guarda o instante para desligar o paraquedas B
        Tia = false;
      }
      if (currentMillis >= InterA) {
        ledState1 = LOW;
        dataString += String("Paraquedas A - Off");                                               // Desliga o paraquedas A
        digitalWrite(IGN_1, ledState1);
      } else {
        ledState1 = HIGH;
        dataString += String("Paraquedas A - On");
        digitalWrite(IGN_1, ledState1);                                                            // Ligou o paraquedas A
      }
      if (currentMillis >= InterB && currentMillis < InterB2) {
        ledState2 = HIGH;
        dataString += String("Paraquedas B - On");                                                // Ligou o parquedas B
        digitalWrite(IGN_2, ledState2);
      } else {
        ledState2 = LOW;
        dataString += String("Paraquedas B - Off");                                               // Desliga paraquedas B
        digitalWrite(IGN_2, ledState2);
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

  File dataFile = SD.open(nome, FILE_WRITE);

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
