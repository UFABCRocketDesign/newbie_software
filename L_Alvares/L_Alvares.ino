#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include <SPI.h>
#include <SD.h>
#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/

float ALT = 0.0;
float i = 0.0;
float Med = 0.0;
float M = 0.0;
float F = 0.0;
float FF = 0.0;
float Vfiltro[11];
float VetApogeu[5];
int A = 0;
int x = 0;
int Queda = 0;


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
  
  String StringC = "";
  StringC += "Temperatura(°C)";
  StringC += "\t";
  StringC += "Pressão(Pa)";
  StringC += "\t";
  StringC +=  "Altitude(m)";
  StringC += "\t";
  StringC += "PressãoNivelMar(Pa)";
  StringC += "\t"; 
  StringC += "AltitudeReal(m)";
  StringC += "\t";
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
    String dataString = "";
    
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
    dataString += String(FF);
    dataString += "\t";
    
    //Apogeu
    VetApogeu[x] = FF;
    x++;
    if (x >= 4)
     {
     x = 0;
     }
     
    if (VetApogeu[x-1] > VetApogeu [x])
     {
     Queda++;
     }
    else
     {
     Queda=0;
     }
     dataString += String(Queda);
     dataString += "\t";
     
    if (Queda >= 4)
     {
      digitalWrite(LED_BUILTIN, HIGH);
      dataString += String("1");
      dataString += "\t";
     }
    else
     {
      dataString += String("0");
      dataString += "\t";
      digitalWrite(LED_BUILTIN, LOW);
     }
     
    //Cartão SD
    Serial.println(dataString);
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

}
