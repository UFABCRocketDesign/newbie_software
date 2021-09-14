#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float ALT = 0.0;
float ALTo = 0.0;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  //Serial.print("Temperatura(°C)  Pressão(Pa) Altitude(m) PressãoNivelMar(Pa)  AltitudeReal(m)");
  Serial.print("Pressão(Pa)");
  Serial.println();
  if (!bmp.begin()) 
  {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}

void loop() 
{
    //Serial.print(bmp.readTemperature());
    //Serial.print("\t");
    //Serial.print(bmp.readPressure());
    //Serial.print("\t");
    //Serial.print(bmp.readAltitude());
    //Serial.print("\t");
    //Serial.print(bmp.readSealevelPressure());
    //Serial.print("\t");
    //Serial.print(bmp.readAltitude(101500));
    //Serial.println();

    ALT = bmp.readAltitude();
    Serial.print(ALT);
    Serial.print("\t");
     
    if(ALT < ALTo)
     {
      digitalWrite(LED_BUILTIN, HIGH);   
      Serial.print(" Foguete Em Queda ");
      Serial.println(); 
     }
     
    ALTo = ALT;
    digitalWrite(LED_BUILTIN, LOW); 
}
