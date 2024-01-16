#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_ADXL345_U.h>
#include <L3G.h>
#include <SPI.h>
#include <SD.h>

#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/
#define AtivarLED2 3000
#define TL 5000
#define PLED1 IGN_1
#define PLED2 IGN_2
#define PLED3 LED_BUILTIN

#define Tam 11
#define Nf 2
#define Vmed 11
#define VEQ 11
#define NP 3

#define MagDbg 0
#define GyrDbg 0
#define AclDbg 0
#define sdDbg 1
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

#define PqDbg (BarDbg && 1)

#define BarTempDbg (BarDbg && 0)
#define BarPresDbg (BarDbg && 0)

#define ApgDbg (BarDbg && 1)

#if BarDbg
Adafruit_BMP085 bmp;
int pos[Tam];
float Mfiltro[Nf][Tam];
float AF[Nf];
float SF[Nf + 1];
float M = 0.0;
float Ap1 = 0.0;
#endif

#if ApgDbg
float AltApg = 0.0;
int CQueda = 0;
int DQueda = 0;
int Q1 = 0;
unsigned long TQ = 0;
#endif

#if PqDbg
int LEDST[NP] = {LOW};
unsigned long TDes[NP];
int PqD[NP] = {LOW};
bool LK[NP] = {false};
bool LC2 = false;
int TP = 1;
unsigned long TAL2 = 0;
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
const int chipSelect = 53;  //pino SD
String NomeArq = "";
#endif

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Ligando os Sensores
#if BarDbg
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif

#if PqDbg
  pinMode(PLED1, OUTPUT);
  pinMode(PLED2, OUTPUT);
  pinMode(PLED3, OUTPUT);
#endif

#if MagDbg
  if (!mag.begin()) {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif

#if GyrDbg
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }
  gyro.enableDefault();
#endif

#if AclDbg
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_16_G);
#endif

#if sdDbg
  int ValorA = 0;
  int NC = 0;
  //Ligando o cartão SD
  while (!Serial) {
    ;
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized.");

  //Formatação do nome do arquivo
  while (NomeArq.length() == 0) {
    String Arq = "";
    String Nome = "LAQ";
    String Zeros = "";
    String VA = "";

    VA = String(ValorA);
    NC = Nome.length() + VA.length();

    for (int a = 0; a < 8 - NC; a++) {
      Zeros += "0";
    }

    Arq += Nome;
    Arq += Zeros;
    Arq += VA;
    Arq += ".txt";

    if (SD.exists(Arq)) {
      Serial.println(Arq + " existe, fornecer outro nome.");
      ValorA++;
    } else {
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
  for (int F = 0; F < Nf; F++) {
    StringC += "Filtro ";
    StringC += String(F + 1);
    StringC += "\t";
  }
#endif

#if ApgDbg
  StringC += "Detecção de Apogeu";
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


#if PqDbg
  StringC += "Ativação Led1";
  StringC += "\t";

  StringC += "Ativação Led2";
  StringC += "\t";

  StringC += "Ativação Led3";
  StringC += "\t";
#endif

  Serial.println(StringC);

#if sdDbg
  File TesteC = SD.open(NomeArq, FILE_WRITE);
  if (TesteC) {
    TesteC.println(StringC);
    TesteC.close();
  }
#endif

#if BarDbg
  //Cálculo da Média
  float Med = 0.0;
  for (int i = 0; i < Vmed; i++) {
    Med = Med + bmp.readAltitude();
  }
  M = (Med / Vmed);
#endif
}

void loop() {
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

  //Impressão dos filtros
#if BarDbg
  SF[0] = (bmp.readAltitude() - M);  //ALT = (bmp.readAltitude() - M);
  dataString += String(SF[0]);       //dataString += String(ALT);
  dataString += "\t";

  for (int IF = 0; IF < Nf; IF++) {
    SF[IF + 1] = Filtragem(IF, SF[IF]);
    dataString += String(SF[IF + 1]);
    dataString += "\t";
  }
#endif

#if ApgDbg
  DQueda = Apogeu(SF[Nf], VEQ);
  if (Q1 == 0 && DQueda == 1) {
    Q1 = 1;
    TQ = TAtual;
  }
  dataString += String(DQueda);
  dataString += "\t";
#endif

  //Captação dos sensores
#if MagDbg
  mag.getEvent(&event);
#if MagXDbg
  dataString += String(event.magnetic.x);
  dataString += "\t";
#endif
#if MagYDbg
  dataString += String(event.magnetic.y);
  dataString += "\t";
#endif
#if MagZDbg
  dataString += String(event.magnetic.z);
  dataString += "\t";
#endif
#endif

#if GyrDbg
  gyro.read();
#if GyrXDbg
  dataString += String((int)gyro.g.x);
  dataString += "\t";
#endif
#if GyrYDbg
  dataString += String((int)gyro.g.y);
  dataString += "\t";
#endif
#if GyrZDbg
  dataString += String((int)gyro.g.z);
  dataString += "\t";
#endif
#endif

#if AclDbg
  accel.getEvent(&event);
#if AclXDbg
  dataString += String(event.acceleration.x);
  dataString += "\t";
#endif
#if AclYDbg
  dataString += String(event.acceleration.y);
  dataString += "\t";
#endif
#if AclZDbg
  dataString += String(event.acceleration.z);
  dataString += "\t";
#endif
#endif

  //Timer e ativação de Parquedas
#if ApgDbg
  if (Q1 == 1)  // se detectar a queda
  {
#if PqDbg

    if (LC2 == false) {
      TAL2 = TQ + AtivarLED2;
      LC2 = true;
    }

    for (int P = 1; P <= NP; P++) {
      if (Q1 == 1 && TP == 1) {
        LK[P] = true;
        PqD[P] = Paraquedas(P, TAtual, LEDST[P], TDes[P]);
        LK[P] = false;
        TP++;
      } else if (TAtual > TAL2 && TP == 2) {
        LK[P] == true;
        PqD[P] = Paraquedas(P, TAtual, LEDST[P], TDes[P]);
        LK[P] = false;
        TP++;
      } else if (SF[Nf] <= -0.25 && TP == 3) {
        LK[P] = true;
        PqD[P] = Paraquedas(P, TAtual, LEDST[P], TDes[P]);
        LK[P] = false;
        TP++;
      } else {
        PqD[P] = Paraquedas(P, TAtual, LEDST[P], TDes[P]);
      }
    }

    digitalWrite(PLED1, PqD[1]);
    digitalWrite(PLED2, PqD[2]);
    digitalWrite(PLED3, PqD[3]);
#endif
  }
#endif


#if PqDbg  //for rodando os paraquedas por PqD[P]

  for (int P = 1; P <= NP; P++) {
    dataString += String(PqD[P]);
    dataString += "\t";
  }
#endif

  Serial.println(dataString);

#if sdDbg
  //Cartão SD
  File dataFile = SD.open(NomeArq, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
#endif
}

//Calculo dos filtros
float Filtragem(int a, float vmd) {
  AF[a] = AF[a] - Mfiltro[a][pos[a]];  //F2 = F2 - Vfiltro2[B];
  Mfiltro[a][pos[a]] = vmd;            //Vfiltro2[B] = SF1;
  AF[a] = AF[a] + Mfiltro[a][pos[a]];  //F2 = F2 + Vfiltro2[B];
  pos[a]++;                            //B++;
  if (pos[a] == Tam)                   //if (B >= 10)
  {                                    //{
    pos[a] = 0;                        //B = 0;
  }                                    //}
  return AF[a] / Tam;                  //SF2 = F2 / 11;
}

//Detecção de Apogeu
int Apogeu(float AltAtual, int VQueda) {
  if (AltApg > AltAtual) {
    CQueda++;
  } else {
    CQueda = 0;
  }
  AltApg = AltAtual;
  return CQueda >= VQueda;
}

//Paraquedas
int Paraquedas(int Par, unsigned long TAt, int StPqd_v[], int TDs[])  //Numero do paraquedas, Tempo atual, Estado do paraquedas, Tempo de desligamento do paraquedas)
{
  if (LK[Par] == true)  //A trava sempre está em false, menos quando ativamos o paraquedas especifico, e isso registra o tempo que precisa ser desligado
  {
    StPqd_v[Par] = HIGH;
    TDs[Par] = TAt + TL;
  }

  if (TAt > TDs[Par])  // roda sempre
  {
    StPqd_v[Par] = LOW;
  }

  return StPqd_v[Par];
}