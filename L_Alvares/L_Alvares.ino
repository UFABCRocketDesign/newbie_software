#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float ALT = 0.0;
//float ALTo = 0.0;
float i = 0.0;
float Med = 0.0;
float M = 0.0;
float N = 1.0;
float Filtro = 0.0;


void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  //Serial.print("Temperatura(°C)  Pressão(Pa) Altitude(m) PressãoNivelMar(Pa)  AltitudeReal(m)");
  //Serial.print("Pressão(Pa)");
  //Serial.println();
  if (!bmp.begin()) 
  {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  for( i = 0; i < 11; i++)
  {
   ALT = bmp.readAltitude();
   Med = Med + ALT;
  }
  M = (Med/11);
  
}

void loop() 
{
    ALT = (bmp.readAltitude() - M);
    Serial.print(ALT);
    Serial.print("\t");

    for (i = 0; i < N; i++)
    {
    Filtro = Filtro + ALT/N; 
    Serial.print(Filtro);
    Serial.print("\t");
    N++;
    }

    Serial.println();  
     
    //if(ALT < ALTo)
    // {
    //  digitalWrite(LED_BUILTIN, HIGH);   
    //  Serial.print(" Foguete Em Queda ");
    // }
    //ALTo = ALT;
    //digitalWrite(LED_BUILTIN, LOW); 
}
