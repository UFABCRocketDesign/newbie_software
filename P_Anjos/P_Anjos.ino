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

float H1 = 0; // Variável global - Não é ressetada a cada loop. Armazena o dado.
float H2 = 0;
float Hmax = 0;
float Soma = 0;
float SomaMov = 0;
float Media = 0;


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
  for (int i = 0; i < 100; i++) {               //Este for serve para definir a altitude da base de lançamento como valor de referência.
    Soma = Soma + bmp.readAltitude();
  }
  Media = Soma / 100;
}

// the loop function runs over and over again forever
void loop() {

  for (int i = 0; i < 10; i++) {              //Este for serve somar os últimos 10 valores medidos.
    SomaMov = SomaMov + bmp.readAltitude();
  }
  MediaMov=SomaMov/10;                        // Média móvel
  H2 = H1;                                    // Guardei a altitude de referência (medição anterior)
  H1 = bmp.readAltitude() - MediaMov;            // Nova leitura de altitude já descontando a altitude da base 


  if (Hmax < H1) {
    Hmax = H1;
  }
  Serial.print(Hmax);
  Serial.print("\t");
  Serial.print(H1);
  Serial.print("\t");
  Delta=Hmax-H1;
  
  if (Delta >= 2) {
    digitalWrite(LED_BUILTIN, HIGH);   // A partir do momento que a diferença de altitude for acima de 3, provavelmente o foguete está descendo.
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
