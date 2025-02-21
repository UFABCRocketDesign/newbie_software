// Aqui você designa o output do LED
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

// Tudo dentro desse statment ficará em loop
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(125);
}