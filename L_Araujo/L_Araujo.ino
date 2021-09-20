//Lídia

//Incluir bibliotecas
#include <Adafruit_BMP085.h>

//Define constantes
#define tam_vetor 100
#define base_descida 35

//Define variáveis globais
Adafruit_BMP085 bmp;

float v[tam_vetor] = {};
float mm[tam_vetor] = {};
float zero = 0;

//Função que devolve quantos valores seguidos tem de queda nas últimas (tam_vetor) contagens
//---------------------------------------
int def_apogeu(float v[tam_vetor]) {
  int desc = 0;
  float b = v[0];

  for (int i = 1; i < tam_vetor; i++) {
    if (v[i] < b) {
      desc++;
      b = v[i];
    } else {
      break;
    }
  }

  return desc;
}

//----------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  Serial.print("Altura  (metros)");

  //Encontra valor para ser retirado da altitude e encontrar a altura (zero) - média dos 100 primeiros valores de sensor
  for (int i = 0; i < tam_vetor; i++) {
    zero += bmp.readAltitude();
  }
  
  zero = zero/tam_vetor;
}

// --------------------------------------
void loop() {
  
    //Identificação da altura (m) - não altitude (bmp.readAltitude)
    float m = bmp.readAltitude() - zero;
    
    Serial.print(m);
    Serial.print('\t');

    //Atualização do vetor para cálculo de média móvel - acrescenta valor novo
    for (int i = 0; i < tam_vetor; i++) {
      v[i - 1] = v[i];
    }
    v[tam_vetor-1] = m;

    //Cálculo da média móvel
    float aux = 0;
    for (int i = 0; i < tam_vetor; i++) {
      aux += v[i];
    }
    
    Serial.println(aux/tam_vetor);

    //Identifica apogeu
    if (def_apogeu(mm) >= base_descida) {
      Serial.print("Atingiu apogeu na altura ");
      Serial.println(v[0]);
      for (int i = 0; i < 3; i++) {
         digitalWrite(LED_BUILTIN, HIGH);
         delay(500);
         digitalWrite(LED_BUILTIN, LOW);
         delay(500);
      }
    }


}
