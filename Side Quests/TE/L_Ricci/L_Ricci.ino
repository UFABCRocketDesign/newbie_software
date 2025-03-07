float pressao = 0.0;
float tensao = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(A0, OUTPUT);

  Serial.println("Transdutor\tPressao");
}

float adcTensao(float adc) {
  tensao = (1023.0/5.0) * adc;
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

  Serial.println();
}
