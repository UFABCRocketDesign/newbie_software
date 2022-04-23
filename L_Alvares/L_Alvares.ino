#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_ADXL345_U.h>
#include <L3G.h>
#include <SPI.h>
#include <SD.h>

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/
#define inter 1000
#define AtivarLED2 3000
#define Tempo 5000
#define PLED1 IGN_1
#define PLED2 IGN_2
#define PLED3 LED_BUILTIN

#define MagDbg 1
#define GyrDbg 1
#define AclDbg 1
#define sdDbg  0
//#define BarDbg 0

//#if BarDbg
Adafruit_BMP085 bmp;
float ALT = 0.0;
float Med = 0.0;
float M = 0.0;
int A = 0;
int B = 0;
float F1 = 0.0;
float F2 = 0.0;
float Vfiltro1[11];
float Vfiltro2[11];
float SF1 = 0.0;
float SF2 = 0.0;
float Ap1 = 0.0;
//#endif

unsigned long TQ = 0;
int Q1 = 0;
int Queda = 0;
int LED1ST = LOW;
boolean LK1 = false;
int LED2ST = LOW;
boolean LK2 = false;
int LED3ST = LOW;
boolean LK3 = false;
unsigned long T1Ant = 0;
unsigned long T2Ant = 0;
unsigned long T3Ant = 0;

#if MagDbg
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
#endif

#if GyrDbg
L3G gyro;
#endif

#if AclDbg
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(1234);
#endif

#if sdDbg
const int chipSelect = 53; //pino SD
String NomeArq = "";
int ValorA = 0;
int NC = 0;
#endif

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  pinMode(PLED1, OUTPUT);
  pinMode(PLED2, OUTPUT);
  pinMode(PLED3, OUTPUT);


  // Ligando os Sensores
//#if BarDbg
  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
//#endif

#if MagDbg
  if (!mag.begin())
  {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1);
  }
#endif

#if GyrDbg
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
#endif

#if AclDbg
  if (!accel.begin())
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_16_G);
#endif

#if sdDbg
  //Ligando o cartão SD
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

  //Formatação do nome do arquivo
  while (NomeArq.length() == 0)
  {
    String Arq = "";
    String Nome = "LAQ";
    String Zeros = "";
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
#endif

  //Cabeçalho
  String StringC = "";

//#if BarDbg
  StringC += "Tempo(s)";
  StringC += "\t";
  StringC += "Altitude(m)";
  StringC += "\t";
  StringC += "Filtro 1(m)";
  StringC += "\t";
  StringC += "Filtro 2(m)";
  StringC += "\t";
//#endif

  StringC += "Detecção de Apogeu";
  StringC += "\t";

#if MagDbg
  StringC += "Magnetômetro X(uT)";
  StringC += "\t";
  StringC += "Magnetômetro Y(uT)";
  StringC += "\t";
  StringC += "Magnetômetro Z(uT)";
  StringC += "\t";
#endif

#if GyrDbg
  StringC += "Giroscópio X(rad/s)";
  StringC += "\t";
  StringC += "Giroscópio Y(rad/s)";
  StringC += "\t";
  StringC += "Giroscópio Z(rad/s)";
  StringC += "\t";
#endif

#if AclDbg
  StringC += "Acelerômetro X(m/s^2)";
  StringC += "\t";
  StringC += "Acelerômetro Y(m/s^2)";
  StringC += "\t";
  StringC += "Acelerômetro Z(m/s^2)";
  StringC += "\t";
#endif

  StringC += "Ativação Led1";
  StringC += "\t";
  StringC += "Ativação Led2";
  StringC += "\t";
  StringC += "Ativação Led3";
  StringC += "\t";

  Serial.print(StringC);

  //StringC += "Temperatura(°C)";
  //StringC += "\t";
  //StringC += "Pressão(Pa)";
  //StringC += "\t";
  //StringC +=  "Altitude(m)";
  //StringC += "\t";
  //StringC += "PressãoNivelMar(Pa)";
  //StringC += "\t";
  //StringC += "AltitudeReal(m)";
  //StringC += "\t";

#if sdDbg
  File TesteC = SD.open(NomeArq , FILE_WRITE);
  if (TesteC)
  {
    TesteC.println(StringC);
    TesteC.close();
  }
#endif

//#if BarDbg
  //Cálculo da Média
  for (int i = 0; i < 11; i++)
  {
    ALT = bmp.readAltitude();
    Med = Med + ALT;
  }
  M = (Med / 11);
//#endif

}

void loop()
{
  String dataString = "";
  unsigned long TAtual = millis();

  //Calculo do tempo
  dataString += String(TAtual / 1000.0);
  dataString += "\t";

  //Calculos dos Filtros
//#if BarDbg
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

  F2 = F2 - Vfiltro2[B];
  Vfiltro2[B] = SF1;
  F2 = F2 + Vfiltro2[B];
  B++;
  if (B >= 10)
  {
    B = 0;
  }
  SF2 = F2 / 11;

  dataString += String(SF1);
  dataString += "\t";
  dataString += String (SF2);
  dataString += "\t";
  Ap1 = SF2;
//#endif

  //Detecção de Apogeu
  if (Ap1 > SF2)
  {
    Queda++;
  }
  else
  {
    Queda = 0;
  }

  if (Queda >= 11)
  {
    Q1 = 1;
    TQ = TAtual;
    dataString += String("1");
    dataString += "\t";
  }
  else
  {
    dataString += String("0");
    dataString += "\t";
  }

  //Captação dos sensores
  sensors_event_t event;

#if MagDbg
  mag.getEvent(&event);
  dataString += String (event.magnetic.x);
  dataString += "\t";
  dataString += String (event.magnetic.y);
  dataString += "\t";
  dataString += String (event.magnetic.z);
  dataString += "\t";
#endif

#if GyrDbg
  gyro.read();
  dataString += String ((int)gyro.g.x);
  dataString += "\t";
  dataString += String ((int)gyro.g.y);
  dataString += "\t";
  dataString += String ((int)gyro.g.z);
  dataString += "\t";
#endif

#if AclDbg
  accel.getEvent(&event);
  dataString += String (event.acceleration.x);
  dataString += "\t";
  dataString += String (event.acceleration.y);
  dataString += "\t";
  dataString += String (event.acceleration.z);
  dataString += "\t";
#endif

  //Timer e ativação de leds
  if (Q1 == 1) // se detectar a queda
  {
    if (LK1 == false) //se a trava estiver desativada
    {
      if (TAtual - T1Ant >= inter) //se o Atual-Anterior > 1 seg, o led liga
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
      if (TAtual - T1Ant >= Tempo) //apos X seg, o Led 1 apaga
      {
        LED1ST = LOW;
      }
    }
    digitalWrite(PLED1, LED1ST);
    if (TAtual - TQ >= AtivarLED2)
    {
      if (LK2 == false) //se a trava estiver desativada
      {
        if (TAtual - T2Ant >= inter) //se o Atual-Anterior > 1 seg, o led liga
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
        if (TAtual - T2Ant >= Tempo)// Caso a trava esteja ativada, Apos X tempo, do Led 2 apaga
        {
          LED2ST = LOW;
        }
      }
      digitalWrite(PLED2, LED2ST);
    }
    if (SF2 <= -0.25 || LK3 == true) // Quando a queda atingir certa altura X, ligar o led
    {
      if (LK3 == false) //se a trava estiver desativada
      {
        if (TAtual - T3Ant >= inter) //se o Atual-Anterior > 1 seg, o led liga
        {
          T3Ant = TAtual;
          LED3ST = HIGH;
        }
        else
        {
          LED3ST = LOW;
        }
        LK3 = true;
      }
      else
      {
        if (TAtual - T3Ant >= Tempo)// Caso a trava esteja ativada, Apos X tempo, do Led 2 apaga
        {
          LED3ST = LOW;
        }
      }
      digitalWrite(PLED3, LED3ST);
    }
  }

  dataString += String(LED1ST);
  dataString += "\t";
  dataString += String(LED2ST);
  dataString += "\t";
  dataString += String(LED3ST);
  dataString += "\t";

Serial.println(dataString);

#if sdDbg
  //Cartão SD
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
#endif

}
