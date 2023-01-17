void setup() {
    pinMode(13, OUTPUT);
}

void loop() {
    digitalWrite(13, HIGH);
    delay(3500);
    digitalWrite(13, LOW);
    delay(3500);
}