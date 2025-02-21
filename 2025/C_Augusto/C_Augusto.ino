#define LED_PISCA 13
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PISCA, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_PISCA, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(180);                       // wait for a second
  digitalWrite(LED_PISCA, LOW);    // turn the LED off by making the voltage LOW
  delay(180);
   digitalWrite(LED_PISCA, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(50);
  digitalWrite(LED_PISCA, LOW);    // turn the LED off by making the voltage LOW
  delay(500); 
  //comentario super original feito por cesar
}