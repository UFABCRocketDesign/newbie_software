#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

// the setup function runs once when you press reset or power the board

// Código para detecção do apogeu de um foguete que utiliza sensor de pressão.

const int chipSelect = 53;   //Define o pino para o chipselect para gravar no cartão SD
float H1 = 0;               // Variável global - Não é ressetada a cada loop. Armazena o dado.
float H2 = 0;
float Hmax = 0;
float Soma = 0;
float SomaMov = 0;
float AltitudeRef = 0;
float MediaMov = 0;
float MediaMA = 0;
float MediaMB = 0;
float Delta;                //Diferença da altitude anterior com a nova que foi medida
float Vetor[3][10];         //Vetor para guardar os últimos 10 valores para a média móvel
float FiltroA[10];          //Segunda filtragem para a média móvel
float FiltroB[10];          // terceira filtragem para a média móvel
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
int ledState = LOW;               // Estado inicial do LED que indica acionamento do paraquedas
unsigned long previousMillis = 0; // Guarda o valor de tempo
const long interval = 2000;       // O intervalo de tempo que o LED deve ficar ligado em milesegundos
bool Aceso= false;                // A variável booleana para verificar se o LED ta ligado

Adafruit_BMP085 bmp;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
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
    c = 8 - (a + b); // Guarda a quantidade de zeros necessária para se colocar entre "PA" e o nº da versão.
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
  Serial.println("Situacao\tApogeu(Hmax)\tAltura filtrada final(H1)\tDelta\tAltura medida no sensor\tTemperature(*C)\tPressure(Pa)\tPressure at sealevel(calculated)(Pa)");//Cabecalho no acompanhamento
  File dataFile = SD.open(nome, FILE_WRITE);
  dataFile.println("Situacao\tApogeu(Hmax)\tAltura filtrada final(H1)\tDelta\tAltura medida no sensor\tTemperature(*C)\tPressure(Pa)\tPressure at sealevel(calculated)(Pa)"); //Cabecalho no SD
  dataFile.close();

  for (int i = 0; i < 100; i++) {             //Este for serve para definir a altitude da base de lancamento como valor de referencia.
    Soma = Soma + bmp.readAltitude();
  }
  AltitudeRef = Soma / 100;
}

// the loop function runs over and over again forever
void loop() {
  String dataString = "";                   // Serve para criar a string que vai guardar os dados para que eles sejam gravados no SD
  SomaMov = 0;
  MediaMov = bmp.readAltitude() - AltitudeRef;
  for (int j = 0; j < 3; j++) {
    for (int i = 8; i >= 0; i--) {        // Laco apenas para a movimentação
      Vetor[j][i + 1] = Vetor[j][i];
    }
    Vetor[j][0] = MediaMov;
    SomaMov = 0;
    for (int i = 0; i < 10; i++) {        // Laco para a somatoria dos valores
      SomaMov = SomaMov + Vetor[j][i];
    }
    MediaMov = SomaMov / 10;
  }
  H2 = H1;                                    // Guardei a altitude de referencia (medicao anterior)
  H1 = MediaMov;                               // Nova leitura de altitude

  if (Hmax < H1) {
    Hmax = H1;
  }
  Delta = Hmax - H1;
  if (ledState == HIGH) {
    Aceso=true;                               // Para garantir que após o acionamento do paraquedas, ele irá executar o próximo if
  }
  if (Delta >= 2|| Aceso == true) {
    unsigned long currentMillis = millis();   //conta em que instante do tempo está
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
        dataString += String("Descendo - Paraquedas On");
        dataString += "\t";
      } else {
        ledState = LOW;
        dataString += String("Descendo - Paraquedas Off");
        dataString += "\t";
      }
      digitalWrite(LED_BUILTIN, ledState);   // A partir do momento que a diferença de altitude for acima de 2, provavelmente o foguete está descendo. Acione o paraquedas
    }

  }
  else {
    //Serial.print("Subindo");
    dataString += String("Subindo");
    dataString += "\t";
  }
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

  File dataFile = SD.open(nome, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening P_ANJOS.txt");
  }
}
