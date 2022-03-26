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

String NomeArq = "";
String Nome = "LAQ";
int ValorA = 0;
int NC = 0;

int T1 = 0;
const int PLED1 = LED_BUILTIN;
int LED1ST = LOW;
unsigned long T1Ant = 0;
const long int1 = 1000;
boolean LK1 = false;

const int PLED2 = IGN_1;
int LED2ST = LOW;
boolean LK2 = false;
unsigned long T2Ant = 0;
const long int2 = 1000;



void setup()
{
  pinMode(PLED1, OUTPUT);
  pinMode(PLED2, OUTPUT);
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

  //Cabeçalho e formatação do nome do arquivo
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

  while (NomeArq.length() == 0)
  {
    String Zeros = "";
    String Arq = "";
    String VA = "";
    VA = String (ValorA);
    NC = Nome.length() + VA.length();

    for (int a = 0; a < 8 - NC; a++)
    {
      Zeros += "0";
    }

    Arq += Nome;
    Arq += Zeros;
    Arq += VA;
    Arq += ".txt";

    if (SD.exists(Arq))
    {
      Serial.println(Arq + " existe, fornecer outro nome.");
      ValorA ++;
    }
    else
    {
      Serial.println(Arq + " esta disponível.");
      NomeArq = Arq;
      break;
    }
  }

  File TesteC = SD.open(NomeArq , FILE_WRITE);
  if (TesteC)
  {
    TesteC.println(StringC);
    TesteC.close();
  }

  //média
  for ( i = 0; i < 11; i++)
  {
    ALT = bmp.readAltitude();
    Med = Med + ALT;
  }
  M = (Med / 11);

}

void loop()
{
  String dataString = "";
  unsigned long TAtual = millis();

  //Calculos filtro 1
  ALT = (bmp.readAltitude() - M);
  dataString += String(ALT);
  dataString += "\t";
  F1 = F1 - Vfiltro1[A];
  Vfiltro1[A] = ALT;
  F1 = F1 + Vfiltro1[A];
  A++;
  if (A >= 10)
  {
    A = 0;
  }
  SF1 = F1 / 11;
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
  SF2 = F2 / 11;
  dataString += String (SF2);
  dataString += "\t";

  //Apogeu
  if (Ap1 > SF2)
  {
    Queda++;
  }
  else
  {
    Queda = 0;
  }
  dataString += String(Queda / 10.0);
  dataString += "\t";

  if (Queda >= 11)
  {
    T1 = 1;
    dataString += String("1");
    dataString += "\t";
  }
  else
  {
    dataString += String("0");
    dataString += "\t";
  }

  //Timer de aviso de queda
  if (T1 == 1) // se detectar a queda
  {
    if (LK1 == false) //se a trava estiver desativada
    {
      if (TAtual - T1Ant >= int1) //se o Atual-Anterior > 1 seg, o led liga
      {
        T1Ant = TAtual;
        LED1ST = HIGH;
      }
      else
      {
        LED1ST = LOW;
      }
      LK1 = true;
    }
    else
    {
      if (TAtual - T1Ant >= 8000) //apos X seg, o Led 1 apaga
      {
        LED1ST = LOW;
      }
    }
    dataString += String(LED1ST);
    dataString += "\t";
    digitalWrite(PLED1, LED1ST);
  }

  if (TAtual >= 3000)
  {
    if (LK2 == false) //se a trava estiver desativada
    {
      if (TAtual - T2Ant >= int2) //se o Atual-Anterior > 1 seg, o led liga
      {
        T2Ant = TAtual;
        LED2ST = HIGH;
      }
      else
      {
        LED2ST = LOW;
      }
      LK2 = true;
    }
    else
    {
      if (TAtual - T2Ant >= 5000)// Caso a trava esteja ativada, Apos X tempo, do Led 2 apaga
      {
        LED2ST = LOW;
      }
    }
    dataString += String(LED2ST);
    dataString += "\t";
    digitalWrite(PLED2, LED2ST);
  }

Ap1 = SF2;

  //Cartão SD
  Serial.println(dataString);
  File dataFile = SD.open(NomeArq , FILE_WRITE);
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
