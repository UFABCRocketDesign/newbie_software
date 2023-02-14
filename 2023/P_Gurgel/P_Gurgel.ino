

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(1, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(1, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(3000);                      // wait for 3 seconds
  digitalWrite(1, LOW);   // turn the LED off by making the voltage LOW
  delay(2000);                      // wait for 2 seconds
}