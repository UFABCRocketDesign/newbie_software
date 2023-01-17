void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the fucking LED on (HIGH is the voltage level)
  delay(902);                       // wait for a darn .902 second
  digitalWrite(LED_BUILTIN, LOW);    // turn the fucking LED off by making the voltage LOW
  delay(902);                       // wait for a darn .902 second
}
