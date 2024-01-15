void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(3000);
  Serial.begin(9600);
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) 
    Serial.print("Temperature = ");
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.println(" meters");
    
    Serial.println();
    delay(500);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(5000);                      // wait for a second
  
}
