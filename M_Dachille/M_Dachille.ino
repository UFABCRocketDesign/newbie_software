#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
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
  Serial.println("Real altitude (m)");
}

void loop() {
  // Piscar o LED
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(10);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(10);                       

  //Segunda coluna - valores
    //temperatura
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
    //Pressão
  Serial.print(bmp.readPressure());
  Serial.print("\t");
    //Altitude
  Serial.print(bmp.readAltitude());
  Serial.print("\t");
    //Pressão Real
  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");
    //Altitude real
  Serial.print(bmp.readAltitude(101500));                       
  
  Serial.println();
}
