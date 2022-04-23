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
#define sdDbg  1
#define TemDbg 1
#define BarDbg 1

#define MagXDbg (MagDbg && 1)
#define MagYDbg (MagDbg && 1)
#define MagZDbg (MagDbg && 1)

#define GyrXDbg (GyrDbg && 1)
#define GyrYDbg (GyrDbg && 1)
#define GyrZDbg (GyrDbg && 1)

#define AclXDbg (AclDbg && 1)
#define AclYDbg (AclDbg && 1)
#define AclZDbg (AclDbg && 1)

#define Led1Dbg (BarDbg && 1)
#define Led2Dbg (BarDbg && 1)
#define Led3Dbg (BarDbg && 1)

#define BarTempDbg (BarDbg && 1)
#define BarPresDbg (BarDbg && 1)

#define ApgDbg (BarDbg && 1)

#if BarDbg
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
#endif

#if ApgDbg
int Queda = 0;
int Q1 = 0;
unsigned long TQ = 0;
#endif

#if Led1Dbg
boolean LK1 = false;
int LED1ST = LOW;
unsigned long T1Ant = 0;
#endif

#if Led2Dbg
boolean LK2 = false;
int LED2ST = LOW;
unsigned long T2Ant = 0;
#endif

#if Led3Dbg
boolean LK3 = false;
int LED3ST = LOW;
unsigned long T3Ant = 0;
#endif

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

  // Ligando os Sensores
#if BarDbg
  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif

#if Led1Dbg
  pinMode(PLED1, OUTPUT);
#endif

#if Led2Dbg
  pinMode(PLED2, OUTPUT);
#endif

#if Led3Dbg
  pinMode(PLED3, OUTPUT);
#endif

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

#if TemDbg
  StringC += "Tempo(s)";
  StringC += "\t";
#endif

#if BarTempDbg
  StringC += "Temperatura(°C)";
  StringC += "\t";
#endif

#if BarPresDbg
  StringC += "Pressão(Pa)";
  StringC += "\t";
#endif

#if BarDbg
  StringC += "Altitude(m)";
  StringC += "\t";
#endif

#if BarDbg
  StringC += "Filtro 1(m)";
  StringC += "\t";
  StringC += "Filtro 2(m)";
  StringC += "\t";
#endif

#if MagXDbg
  StringC += "Magnetômetro X(uT)";
  StringC += "\t";
#endif
#if MagYDbg
  StringC += "Magnetômetro Y(uT)";
  StringC += "\t";
#endif
#if MagZDbg
  StringC += "Magnetômetro Z(uT)";
  StringC += "\t";
#endif

#if GyrXDbg
  StringC += "Giroscópio X(rad/s)";
  StringC += "\t";
#endif
#if GyrYDbg
  StringC += "Giroscópio Y(rad/s)";
  StringC += "\t";
#endif
#if GyrZDbg
  StringC += "Giroscópio Z(rad/s)";
  StringC += "\t";
#endif

#if AclXDbg
  StringC += "Acelerômetro X(m/s^2)";
  StringC += "\t";
#endif
#if AclYDbg
  StringC += "Acelerômetro Y(m/s^2)";
  StringC += "\t";
#endif
#if AclZDbg
  StringC += "Acelerômetro Z(m/s^2)";
  StringC += "\t";
#endif

#if ApgDbg
  StringC += "Detecção de Apogeu";
  StringC += "\t";
#endif

#if Led1Dbg
  StringC += "Ativação Led1";
  StringC += "\t";
#endif

#if Led2Dbg
  StringC += "Ativação Led2";
  StringC += "\t";
#endif

#if Led3Dbg
  StringC += "Ativação Led3";
  StringC += "\t";
#endif

  Serial.println(StringC);

#if sdDbg
  File TesteC = SD.open(NomeArq , FILE_WRITE);
  if (TesteC)
  {
    TesteC.println(StringC);
    TesteC.close();
  }
#endif

#if BarDbg
  //Cálculo da Média
  for (int i = 0; i < 11; i++)
  {
    ALT = bmp.readAltitude();
    Med = Med + ALT;
  }
  M = (Med / 11);
#endif

}

void loop()
{
  String dataString = "";
  unsigned long TAtual = millis();
  sensors_event_t event;
  
  //Calculo do tempo
#if TemDbg
  dataString += String(TAtual / 1000.0);
  dataString += "\t";
#endif

  // Temperatura e pressão
#if BarTempDbg
  dataString += String(bmp.readTemperature());
  dataString += "\t";
#endif

#if BarPresDbg
  dataString += String(bmp.readPressure());
  dataString += "\t";
#endif

  //Calculos dos Filtros
#if BarDbg
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
#endif

  //Detecção de Apogeu
#if ApgDbg
  if (Ap1 > SF2)
  {
    Queda++;
  }
  else
  {
    Queda = 0;
  }
#endif

  //Captação dos sensores
#if MagDbg
  mag.getEvent(&event);
#if MagXDbg
  dataString += String (event.magnetic.x);
  dataString += "\t";
#endif
#if MagYDbg
  dataString += String (event.magnetic.y);
  dataString += "\t";
#endif
#if MagZDbg
  dataString += String (event.magnetic.z);
  dataString += "\t";
#endif
#endif

#if GyrDbg
  gyro.read();
#if GyrXDbg
  dataString += String ((int)gyro.g.x);
  dataString += "\t";
#endif
#if GyrYDbg
  dataString += String ((int)gyro.g.y);
  dataString += "\t";
#endif
#if GyrZDbg
  dataString += String ((int)gyro.g.z);
  dataString += "\t";
#endif
#endif

#if AclDbg
  accel.getEvent(&event);
#if AclXDbg
  dataString += String (event.acceleration.x);
  dataString += "\t";
#endif
#if AclYDbg
  dataString += String (event.acceleration.y);
  dataString += "\t";
#endif
#if AclZDbg
  dataString += String (event.acceleration.z);
  dataString += "\t";
#endif
#endif

#if ApgDbg
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
#endif

  //Timer e ativação de leds
#if ApgDbg
  if (Q1 == 1) // se detectar a queda
  {
#if Led1Dbg
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
    dataString += String(LED1ST);
    dataString += "\t";
#endif
#if Led2Dbg
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
    dataString += String(LED2ST);
    dataString += "\t";
#endif
#if Led3Dbg
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
    dataString += String(LED3ST);
    dataString += "\t";
#endif
  }
#endif

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
