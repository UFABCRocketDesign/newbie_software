#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}

void loop() {
  // Piscar o LED
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(1000);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(1500);                       

  // Primeira coluna - medidas
    //temperatura
  Serial.print("Temperature (*C)");
  Serial.print("\t");
    //Pressão
  Serial.print("Pressure (Pa)");
  Serial.print("\t");
    //Altitude
  Serial.print("Altitude (m)");
  Serial.print("\t");
    // Pressão real
  Serial.print("Pressure at sealevel (calculated)(Pa)");
  Serial.print("\t");
    // Altitude Real
  Serial.print("Real altitude (m)");
  Serial.print("\t");
  Serial.print("\t");
  Serial.print("\t");
  Serial.print("\t");
  //Segunda coluna - valores
    //temperatura
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
  Serial.print("\t");
    //Pressão
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  Serial.print("\t");
    //Altitude
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");
  Serial.print("\t");
    //Pressão Real
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");
  Serial.print("\t");
    //Altitude real
  Serial.print(bmp.readAltitude(101500));                       
  Serial.println(" meters");


  
  Serial.println();
  delay(1000);
}
