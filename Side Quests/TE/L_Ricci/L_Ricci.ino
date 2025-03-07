float pressao = 0.0;
float tensao = 0.0;

void setup() {
  Serial.begin(9600);
  Serial.println("Transdutor\tPressao");
}

float adcTensao(float adc) {
  tensao = (1023/5) * adc;
  return tensao;
}

float tensaoPressao(float tensao) {
  pressao = 125 * tensao;
  return pressao;
}

void loop() {
 
  float transdutor = analogRead(A0);

  Serial.print(transdutor);
  Serial.print("\t");
  
  tensao = adcTensao(transdutor);
  Serial.print(tensaoPressao(tensao));
}
