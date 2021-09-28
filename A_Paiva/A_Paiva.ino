#include <Adafruit_BMP085.h>

#define tam1 15        //Tamanho do vetor do filtro1
#define tam2 15        //Tamanho do vetor do filtro2

float H11 = 0;         //Valor da altitude atual do filtro1 (MediaMov)
float H12 = 0;         //Valor da altitude atual do filtro2 

float H21 = 0;         //Valor anterior ao do atual do filtro1
float H22 = 0;         //Valor anterior ao do atual do filtro2

float Hmax1 = 0;       //Valor máximo do filtro1
float Hmax2 = 0;       //Valor máximo do filtro2

float SomaRef = 0;     //Soma valores iniciais(foguete parado na base)
float AltitudeRef = 0; //É o valor da média dos valores iniciais(foguete parado na base)

float SomaMov1 = 0;    //Soma dos valores do vetor do filtro1
float SomaMov2 = 0;    //Soma dos valores do vetor do filtro2

float MediaMov1 = 0;   //É o valor da média dos valores do vetor do filtro1
float MediaMov2 = 0;   //É o valor da média dos valores do vetor do filtro2

float Delta1;          //Diferença entre valor máximo do filtro (Hmax) e valor atual referênciado (H1)
float Delta2;          //Diferença entre valor máximo do filtro (Hmax) e valor atual referênciado (H1)
 
float Vetor1[tam1];    //Vetor para guardar os valores para a média utilizada pelo filtro, vai de 0 a tam-1
float Vetor2[tam2];    //Vetor para guardar os valores para a média utilizada pelo filtro, vai de 0 a tam-1

int led = 0;           //Variável para funcionamento do LED  

Adafruit_BMP085 bmp;   //Cria variável 'bmp' para a biblioteca Adafruit_BMP085

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
  SomaMov1=0;                                 //Zera o SomaMov1 em todo loop
  SomaMov2=0;                                 //Zera o SomaMov2 em todo loop
  //Filtro 1
  for (int i = tam1-2; i>=0; i--){            //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1 de 0 para tam1-1
   Vetor1[i+1]= Vetor1[i];                    
  }
  Vetor1[0]=bmp.readAltitude()-AltitudeRef;   //Esse é o valor mais atualizado do filtro1 
  for (int i = 0; i < tam1; i++) {            //Esse 'for' faz a soma dos últimos 10 valores medidos, para a média do filtro1
    SomaMov1=SomaMov1+Vetor1[i];
  }
  MediaMov1=SomaMov1/tam1;                        
  H21 = H11;                                    
  H11 = MediaMov1;                              
  if (Hmax1 < H11) {                          //Pega o valor máximo da média/filtro1
    Hmax1 = H11;
  }
  //Filtro 2
  for (int i = tam2-2; i>=0; i--){            //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1 de 0 para tam1-1
   Vetor2[i+1]= Vetor2[i];                    
  }
  Vetor2[0]=H11;                              //Esse é o valor mais atualizado do filtro1 
  for (int i = 0; i < tam2; i++) {            //Esse 'for' faz a soma dos últimos 10 valores medidos, para a média do filtro1
    SomaMov2=SomaMov2+Vetor2[i];
  }
  MediaMov2=SomaMov2/tam2;                        
  H22 = H12;                                    
  H12 = MediaMov2;                              
  if (Hmax2 < H12) {                            //Pega o valor máximo da média/filtro1
    Hmax2 = H12;
  }

  Delta1=Hmax1-H11;                              //Compara o valor máximo do filtro1 com o valor atual do filtro1
  Delta2=Hmax2-H12;                              //Compara o valor máximo do filtro2 com o valor atual do filtro2
  
  Serial.print(Hmax2);
  Serial.print("\t");
  Serial.print(H11);
  Serial.print("\t");
  Serial.print(H12);
  Serial.print("\t");
  Serial.print(Vetor1[0]);
  Serial.print("\t");

  
  if (Delta2 >= 2) {                           //Quando a diferença de altitude for acima de 2 (metros), provavelmente o foguete está descendo ou pode haver um controle de quando se quer que abra o paraquedas
    digitalWrite(LED_BUILTIN, HIGH);
    led = 1;
    Serial.print("Descendo");
    Serial.print("\t");
    Serial.print(led);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    led = 0;
    Serial.print("Subindo");
    Serial.print("\t");
    Serial.print(led);
  }
  Serial.println();
}
