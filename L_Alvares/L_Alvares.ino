#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include <SPI.h>
#include <SD.h>
#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/

float ALT = 0.0;
//float ALTo = 0.0;
float i = 0.0;
float Med = 0.0;
float M = 0.0;
float F = 0.0;
float FF = 0.0;
float Vfiltro[11];
int A = 0;

const int chipSelect = 53;



void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  //ligando bmp
  if (!bmp.begin()) 
  {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  
  //ligando SD
  while (!Serial) 
  {
    ;
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized.");

  //Cabeçalho
  
  //Serial.print("Temperatura(°C)  Pressão(Pa) Altitude(m) PressãoNivelMar(Pa)  AltitudeReal(m)");
  //Serial.print("Pressão(Pa)");
  //Serial.print("\t"); 
  //Serial.println();
  
  String StringC = "";
  StringC = String("Temperatura(°C)") + ";" +  String("Pressão(Pa)")+ ";" +  String("Altitude(m)")+ ";" +  String("PressãoNivelMar(Pa)") + ";" +  String("AltitudeReal(m)") ;
  File TesteC = SD.open("Alvares.txt", FILE_WRITE);
  if (TesteC)
  {
  TesteC.println(StringC);
  TesteC.close();
  }
 
  //média
  for( i = 0; i < 11; i++)
  {
   ALT = bmp.readAltitude();
   Med = Med + ALT;
  }
  M = (Med/11);
  
}

void loop() 
{
    //Calculos filtro
    ALT = (bmp.readAltitude() - M);
    F = F - Vfiltro[A];
    Vfiltro[A] = ALT;
    F = F + Vfiltro[A];
    A++;
    if(A >= 10)
     {
      A = 0;
     }
    FF = F/11;
    String dataString = "";
    dataString += String(FF);
    dataString += "\t";
    Serial.println(dataString);

   //Cartão SD
   File dataFile = SD.open("Alvares.txt", FILE_WRITE);
   if (dataFile) 
   {
    dataFile.println(dataString);
    dataFile.close();
   }
   else 
   {
    Serial.println("error opening datalog.txt");
   }
  
    
    //if(ALT < ALTo)
    // {
    //  digitalWrite(LED_BUILTIN, HIGH);   
    //  Serial.print(" Foguete Em Queda ");
    // }
    //digitalWrite(LED_BUILTIN, LOW); 
}
