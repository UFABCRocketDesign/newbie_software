#include <Adafruit_BMP085.h>

float H1 = 0;         //Valor da altitude atual do filtro (MediaMov)
float H2 = 0;         //Valor anterior ao do atual do filtro
float Hmax = 0;       //Valor máximo do filtro
float SomaRef = 0;    //Soma valores iniciais(foguete parado na base)
float AltitudeRef = 0;//É o valor da média dos valores iniciais(foguete parado na base)
float SomaMov = 0;    //Soma dos valores do vetor do filtro
float MediaMov = 0;   //É o valor da média dos valores do vetor do filtro
float Delta;          //Diferença entre valor máximo do filtro (Hmax) e valor atual referênciado (H1) 
float Vetor[10];      //Vetor para guardar os valores para a média utilizada pelo filtro, vai de 0 a 9

Adafruit_BMP085 bmp;  //Cria variável 'bmp' para a biblioteca Adafruit_BMP085

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  Serial.println("Temperature(*C)\tPressure(Pa)\tAltitude(m)\tPressure at sealevel(calculated)(Pa)\tReal altitude(m)");
  for (int i = 0; i < 100; i++) {             //Este 'for' serve para definir a altitude da base de lançamento como valor de referência.
    SomaRef = SomaRef + bmp.readAltitude();
  }
  AltitudeRef = SomaRef / 100;
}
void loop() {
  SomaMov=0;                                  //Zera o SomaMov em todo loop
  for (int i = 8; i>=0; i--){                 //Esse 'for' anda com os valores do vetor do filtro de 1 em 1 de 0 para 9
   Vetor[i+1]= Vetor[i];                      
  }
  Vetor[0]=bmp.readAltitude()-AltitudeRef;    //Esse é o valor mais atualizado e que sempre será atualizado 
  for (int i = 0; i < 10; i++) {              //Esse 'for' faz a soma dos últimos 10 valores medidos, para a média do filtro
    SomaMov=SomaMov+Vetor[i];
  }
  MediaMov=SomaMov/10;                        
  H2 = H1;                                    
  H1 = MediaMov;                              
  if (Hmax < H1) {                            //Pega o valor máximo da média ou filtro
    Hmax = H1;
  }
  Delta=Hmax-H1;                              //Compara o valor máximo do filtro com o valor atual referênciado
  Serial.print(Hmax);
  Serial.print("\t");
  Serial.print(H1);
  Serial.print("\t");
  Serial.print(Vetor[0]);
  Serial.print("\t");         
  Serial.print(Delta);
  Serial.print("\t");
  if (Delta >= 2) {                           //Quando a diferença de altitude for acima de 2 (metros), provavelmente o foguete está descendo.
    digitalWrite(LED_BUILTIN, HIGH);          
    //Serial.print("\t");
    //Serial.print(Delta);
    //Serial.print("\t");
    Serial.print("Descendo");
  }
  else {
    Serial.print("Subindo");
  }
  Serial.println();
}
