#include <Adafruit_BMP085.h>

float H1 = 0;      // Variável global - Não é ressetada a cada loop. Armazena o dado.
float H2 = 0;
float Hmax = 0;
float Soma = 0;
float SomaMov = 0;
float AltitudeRef = 0;
float MediaMov = 0;
float Delta;
float Vetor[10];     //Vetor para guardar os últimos 10 valores para a média móvel

Adafruit_BMP085 bmp;

void setup() {
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
  AltitudeRef = Soma / 100;
}
void loop() {
  SomaMov=0;
  for (int i = 8; i>=0; i--){
   Vetor[i+1]= Vetor[i];                      //Esse Vetor serve para guardar os valores
  }
  Vetor[0]=bmp.readAltitude()-AltitudeRef;    // Esse é o valor que será atualizado sempre 
  for (int i = 0; i < 10; i++) {              //Este for serve somar os últimos 10 valores medidos.
    SomaMov=SomaMov+Vetor[i];
  }
  MediaMov=SomaMov/10;                        // Média móvel
  H2 = H1;                                    // Guardei a altitude de referência (medição anterior)
  H1 = MediaMov;                              // Nova leitura de altitude
  if (Hmax < H1) {
    Hmax = H1;
  }
  Serial.print(Hmax);
  Serial.print("\t");
  Serial.print(H1);
  Serial.print("\t");
  Serial.print(Vetor[0]);
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
