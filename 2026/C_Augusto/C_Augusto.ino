void setup() {
  // inicializa a funcao do pino do led 13
  pinMode(LED_BUILTIN, OUTPUT);
}

// loopzao pra repetir
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // muda o pino pra alta tensao
  delay(500);                      // segura 0.5 segundos
  digitalWrite(LED_BUILTIN, LOW);   // muda o pino pra baixa tensao
  delay(500);                      // segura 0.5 segundos
}
