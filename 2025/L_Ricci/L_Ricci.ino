// Aqui você designa o output do LED
void setup() {
  pinMode(13, OUTPUT);
}

// Tudo dentro desse statment ficará em loop
void loop() {
  // Liga o LED
  digitalWrite(13, HIGH);
  // "Dormir" por 1s
  delay(1000);
  // Desliga o LED
  digitalWrite(13,LOW);
  // "Dorme" por 1s
  delay(1000);
}