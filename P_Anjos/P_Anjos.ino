#include <Adafruit_BMP085.h>
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

float H1 = 0;      // Variável global - Não é ressetada a cada loop. Armazena o dado.
float H2 = 0;
float Hmax = 0;
float Soma = 0;
float SomaMov = 0;
float AltitudeRef = 0;
float MediaMov = 0;
float MediaMA = 0;
float MediaMB = 0;    
float Delta;            //Diferença da altitude anterior com a nova que foi medida 
float Vetor[3][10];     //Vetor para guardar os últimos 10 valores para a média móvel
float FiltroA[10];      //Segunda filtragem para a média móvel
float FiltroB[10];      // terceira filtragem para a média móvel
float SomaFA = 0;
float SomaFB = 0;


Adafruit_BMP085 bmp;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  Serial.println("Temperature(*C)\tPressure(Pa)\tAltitude(m)\tPressure at sealevel(calculated)(Pa)\tReal altitude(m)");
  for (int i = 0; i < 100; i++) {             //Este for serve para definir a altitude da base de lancamento como valor de referencia.
    Soma = Soma + bmp.readAltitude();
  }
  AltitudeRef = Soma / 100;
}

// the loop function runs over and over again forever
void loop() {


  SomaMov=0;
  SomaFA=0;
  SomaFB=0; 
  for (int i = 8; i>=0; i--){
   Vetor[0][i+1]= Vetor[0][i];                      //Esse Vetor serve para guardar os valores. Preciso usar os ultimos 10 valores medidos e por isso preciso registrar aos poucos
  }
  Vetor[0][0]=bmp.readAltitude()-AltitudeRef;    // Esse e o valor que sera atualizado sempre 
  for (int i = 0; i < 10; i++) {              //Este for serve somar os ultimos 10 valores medidos.
    SomaMov=SomaMov+Vetor[0][i];
  }
  MediaMov=SomaMov/10;                        // Media movel (para a filtragem)
 // ETAPA DE FILTRAGEM 
  for (int i = 8; i>=0; i--){
   Vetor[1][i+1]= Vetor[1][i];         
  }
  Vetor[1][0]=MediaMov;                        // Esse e o valor que sera atualizado sempre 
  for (int i = 0; i < 10; i++) {              //Este for serve somar os ultimos 10 valores medidos.
    SomaFA=SomaFA+Vetor[1][i];
  }
  MediaMA=SomaFA/10;                          
  for (int i = 8; i>=0; i--){
   Vetor[2][i+1]= Vetor[2][i];         
  }
  Vetor[2][0]=MediaMA;                        // Esse e o valor que sera atualizado sempre 
  for (int i = 0; i < 10; i++) {              //Este for serve somar os ultimos 10 valores medidos.
    SomaFB=SomaFB+Vetor[2][i];
  }
  MediaMB=SomaFB/10;  
  H2 = H1;                                    // Guardei a altitude de referencia (medicao anterior)
  H1 = MediaMB;                               // Nova leitura de altitude


  if (Hmax < H1) {
    Hmax = H1;
  }
  Serial.print(Hmax);
  Serial.print("\t");
  Serial.print(H1);
  Serial.print("\t");
  Serial.print(MediaMA);
  Serial.print("\t");
  Serial.print(MediaMov);
  Serial.print("\t");
  Serial.print(Vetor[0][0]);
  Serial.print("\t");
  Delta=Hmax-H1;
  
  if (Delta >= 2) {
    digitalWrite(LED_BUILTIN, HIGH);   // A partir do momento que a diferença de altitude for acima de 2, provavelmente o foguete está descendo.
    Serial.print("\t");
    Serial.print(Delta);
    Serial.print("\t");
    Serial.print("Descendo");
  }
  else {
    Serial.print("Subindo");
  }
  Serial.println();

}
