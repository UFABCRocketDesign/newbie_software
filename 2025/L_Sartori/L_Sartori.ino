#define LED_PISCA 13
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PISCA, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_PISCA, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1500);                       // wait for a second
  digitalWrite(LED_PISCA, LOW);    // turn the LED off by making the voltage LOW
  delay(500);     // wait for a second
  digitalWrite(LED_PISCA, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(3000);                       // wait for a second
  digitalWrite(LED_PISCA, LOW);    // turn the LED off by making the voltage LOW
  delay(250);   
}
