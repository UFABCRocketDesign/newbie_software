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
float F1 = 0.0;
float SF1 = 0.0;
float Vfiltro1[11];
float F2 = 0.0;
float SF2 = 0.0;
float Vfiltro2[11];
float Ap1 = 0.0;
int A = 0;
int B = 0;
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
    
    //Calculos filtro 1
    ALT = (bmp.readAltitude() - M);
    dataString += String(ALT);
    dataString += "\t";
    F1 = F1 - Vfiltro1[A];
    Vfiltro1[A] = ALT;
    F1 = F1 + Vfiltro1[A];
    A++;
    if(A >= 10)
     {
      A = 0;
     }
    SF1 = F1/11;
    dataString += String(SF1);
    dataString += "\t";

    //Calculos filtro 2 
    F2 = F2 - Vfiltro2[B];
    Vfiltro2[B] = SF1;
    F2 = F2 + Vfiltro2[B];
    B++;
    if (B >= 10)
    {
      B = 0;
    }
    SF2 = F2/11;
    dataString += String (SF2);
    dataString += "\t";
     
    //Apogeu  
    if (Ap1 > SF2)
     {
     Queda++;
     }
    else
     {
     Queda=0;
     }
     dataString += String(Queda/10.0);
     dataString += "\t";
     
    if (Queda >= 11)
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

    Ap1 = SF2 ;
     
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
