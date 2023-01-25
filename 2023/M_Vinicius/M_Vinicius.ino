// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE)

void setup() {
  Serial.begin (9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
}

void loop() {
  Serial.print ("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print ("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  // Calcular altitude 
  
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");

  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");

  Serial.print("Real altitude = ");
  Serial.print(bmp.readAltitude(101500));
  Serial.println("meters");

  Serial.println();
  delay(500);

}