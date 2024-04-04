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

#define TL 5000

#define TAM 11  // Quantidade de dados a serem avaliados
#define NF 2    // N° de filtros
#define VEQ 11  // Comparativo para detectar queda
#define NP 4    // N° de paraquedas

#define MAG_DBG 0
#define GYR_DBG 0
#define ACL_DBG 0
#define SD_DBG 0
#define TEM_DBG 1
#define BAR_DBG 1

#define USE_MAG_X (MAG_DBG && 1)
#define USE_MAG_Y (MAG_DBG && 1)
#define USE_MAG_Z (MAG_DBG && 1)

#define USE_GYR_X (GYR_DBG && 1)
#define USE_GYR_Y (GYR_DBG && 1)
#define USE_GYR_Z (GYR_DBG && 1)

#define USE_ACL_X (ACL_DBG && 1)
#define USE_ACL_Y (ACL_DBG && 1)
#define USE_ACL_Z (ACL_DBG && 1)

#define PQ_DBG (BAR_DBG && 1)

#define USE_BAR_TEMP (BAR_DBG && 0)
#define USE_BAR_PRES (BAR_DBG && 0)

#define APG_DBG (BAR_DBG && 1)

#if BAR_DBG
Adafruit_BMP085 bmp;
int pos[TAM];
float mFiltro[NF][TAM]; //
float af[NF];      // Valor de entrada no filtro
float sf[NF + 1];  // Valor de saida do filtro
float m = 0.0;     // Valor da média final
#endif

#if APG_DBG
float altApg = 0.0;   // Altura do Apogeu do foguete
int cQueda = 0;       // Contador para verificar se o foguete está em queda
int dQueda = 0;       // Variável registradora de queda (ou não)
int q1 = 0;           // Trava de queda para outras funções
unsigned long tq = 0; // Tempo onde detectou a queda
#endif

#if PQ_DBG

const int leds[] = { IGN_1, IGN_2, IGN_3, IGN_4 };  // Vetor que relaciona os paraquedas
int pqd[NP] = { LOW };                              // Vetor de estado dos paraquedas
const int atraso[NP] = { 0, 3000, 0, 3000 };        // Tempo de delay para acionar os paraquedas
float hMin[NP] = { 0, 0, -5, -5 };                  // Altura minima para acionar os paraquedas 
int lock[NP] = { 0 };                               // Trava "anti-reacionamento" dos paraquedas
unsigned long ta[NP];                               // Instante de tempo em que o paraquedas deve ser ativado
unsigned long tDes[NP];                             // Instante de tempo em que o paraquedas deve ser desativado
#endif

#if MAG_DBG
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
#endif

#if GYR_DBG
L3G gyro;
#endif

#if ACL_DBG
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(1234);
#endif

#if SD_DBG
const int chipSelect = 53;  //pino SD
String nomeArq = "";
#endif

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Ligando os Sensores
#if BAR_DBG
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
#endif

#if PQ_DBG
  for (int i = 0; i <= NP; i++) {
    pinMode(leds[i], OUTPUT);
  }
#endif

#if MAG_DBG
  if (!mag.begin()) {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
#endif

#if GYR_DBG
  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }
  gyro.enableDefault();
#endif

#if ACL_DBG
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_16_G);
#endif

#if SD_DBG
  int valorA = 0;
  int nc = 0;
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
  while (nomeArq.length() == 0) {
    String Arq = "";
    String Nome = "LAQ";
    String Zeros = "";
    String VA = "";

    VA = String(valorA);
    nc = Nome.length() + VA.length();

    for (int a = 0; a < 8 - nc; a++) {
      Zeros += "0";
    }

    Arq += Nome;
    Arq += Zeros;
    Arq += VA;
    Arq += ".txt";

    if (SD.exists(Arq)) {
      Serial.println(Arq + " existe, fornecer outro nome.");
      valorA++;
    } else {
      Serial.println(Arq + " esta disponível.");
      nomeArq = Arq;
      break;
    }
  }
#endif

  //Cabeçalho
  String StringC = "";

#if TEM_DBG
  StringC += "Tempo(s)";
  StringC += "\t";
#endif

#if USE_BAR_TEMP
  StringC += "Temperatura(°C)";
  StringC += "\t";
#endif

#if USE_BAR_PRES
  StringC += "Pressão(Pa)";
  StringC += "\t";
#endif

#if BAR_DBG
  StringC += "Altitude(m)";
  StringC += "\t";
#endif

#if BAR_DBG
  for (int F = 0; F < NF; F++) {
    StringC += "Filtro ";
    StringC += String(F + 1);
    StringC += "\t";
  }
#endif

#if APG_DBG
  StringC += "Detecção de Apogeu";
  StringC += "\t";
#endif

#if USE_MAG_X
  StringC += "Magnetômetro X(uT)";
  StringC += "\t";
#endif
#if USE_MAG_Y
  StringC += "Magnetômetro Y(uT)";
  StringC += "\t";
#endif
#if USE_MAG_Z
  StringC += "Magnetômetro Z(uT)";
  StringC += "\t";
#endif

#if USE_GYR_X
  StringC += "Giroscópio X(rad/s)";
  StringC += "\t";
#endif
#if USE_GYR_Y
  StringC += "Giroscópio Y(rad/s)";
  StringC += "\t";
#endif
#if USE_GYR_Z
  StringC += "Giroscópio Z(rad/s)";
  StringC += "\t";
#endif

#if USE_ACL_X
  StringC += "Acelerômetro X(m/s^2)";
  StringC += "\t";
#endif
#if USE_ACL_Y
  StringC += "Acelerômetro Y(m/s^2)";
  StringC += "\t";
#endif
#if USE_ACL_Z
  StringC += "Acelerômetro Z(m/s^2)";
  StringC += "\t";
#endif


#if PQ_DBG
  StringC += "Ativação Led1";
  StringC += "\t";

  StringC += "Ativação Led2";
  StringC += "\t";

  StringC += "Ativação Led3";
  StringC += "\t";

  StringC += "Ativação Led4";
  StringC += "\t";
#endif

  Serial.println(StringC);

#if SD_DBG
  File TesteC = SD.open(nomeArq, FILE_WRITE);
  if (TesteC) {
    TesteC.println(StringC);
    TesteC.close();
  }
#endif

#if BAR_DBG
  //Cálculo da Média
  float med = 0.0;
  for (int i = 0; i < TAM; i++) {
    med = med + bmp.readAltitude();
  }
  m = (med / TAM);
#endif
}

void loop() {
  String dataString = "";
  unsigned long tAtual = millis();
  sensors_event_t event;

  //Calculo do tempo
#if TEM_DBG
  dataString += String(tAtual / 1000.0);
  dataString += "\t";
#endif

  // Temperatura e pressão
#if USE_BAR_TEMP
  dataString += String(bmp.readTemperature());
  dataString += "\t";
#endif

#if USE_BAR_PRES
  dataString += String(bmp.readPressure());
  dataString += "\t";
#endif

  //Impressão dos filtros
#if BAR_DBG
  sf[0] = (bmp.readAltitude() - m);  //ALT = (bmp.readAltitude() - m);
  dataString += String(sf[0]);       //dataString += String(ALT);
  dataString += "\t";

  for (int IF = 0; IF < NF; IF++) {
    sf[IF + 1] = Filtragem(IF, sf[IF]);
    dataString += String(sf[IF + 1]);
    dataString += "\t";
  }
#endif

#if APG_DBG
  dQueda = Apogeu(sf[NF], VEQ);
  if (q1 == 0 && dQueda == 1) {
    q1 = 1;
    tq = tAtual;
  }
  dataString += String(dQueda);
  dataString += "\t";
#endif

  //Captação dos sensores
#if MAG_DBG
  mag.getEvent(&event);
#if USE_MAG_X
  dataString += String(event.magnetic.x);
  dataString += "\t";
#endif
#if USE_MAG_Y
  dataString += String(event.magnetic.y);
  dataString += "\t";
#endif
#if USE_MAG_Z
  dataString += String(event.magnetic.z);
  dataString += "\t";
#endif
#endif

#if GYR_DBG
  gyro.read();
#if USE_GYR_X
  dataString += String((int)gyro.g.x);
  dataString += "\t";
#endif
#if USE_GYR_Y
  dataString += String((int)gyro.g.y);
  dataString += "\t";
#endif
#if USE_GYR_Z
  dataString += String((int)gyro.g.z);
  dataString += "\t";
#endif
#endif

#if ACL_DBG
  accel.getEvent(&event);
#if USE_ACL_X
  dataString += String(event.acceleration.x);
  dataString += "\t";
#endif
#if USE_ACL_Y
  dataString += String(event.acceleration.y);
  dataString += "\t";
#endif
#if USE_ACL_Z
  dataString += String(event.acceleration.z);
  dataString += "\t";
#endif
#endif

  //Timer e ativação de Parquedas
#if APG_DBG
  if (q1 == 1)  // se detectar a queda
  {
#if PQ_DBG
    for (int P = 0; P < NP; P++) {


      if ((P < NP / 2) && lock[P] == 0) {
        ta[P] = tq + atraso[P];
      } else if (sf[NF] <= -10 && lock[P] == 0) {
        ta[P] = tAtual + atraso[P];
      }


      pqd[P] = Paraquedas(P, tAtual);  //Passar os parâmetros necessários
      digitalWrite(leds[P], pqd[P]);
    }
#endif
  }
#endif


#if PQ_DBG  //for rodando os paraquedas por pqd[P]
  for (int P = 0; P < NP; P++) {
    dataString += String(pqd[P]);
    dataString += "\t";
  }
#endif

  Serial.println(dataString);

#if SD_DBG
  //Cartão SD
  File dataFile = SD.open(nomeArq, FILE_WRITE);
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
  af[a] = af[a] - mFiltro[a][pos[a]];  //F2 = F2 - Vfiltro2[B];
  mFiltro[a][pos[a]] = vmd;            //Vfiltro2[B] = SF1;
  af[a] = af[a] + mFiltro[a][pos[a]];  //F2 = F2 + Vfiltro2[B];
  pos[a]++;                            //B++;
  if (pos[a] == TAM)                   //if (B >= 10)
  {                                    //{
    pos[a] = 0;                        //B = 0;
  }                                    //}
  return af[a] / TAM;                  //SF2 = F2 / 11;
}

//Detecção de Apogeu
int Apogeu(float AltAtual, int VQueda) {
  if (altApg > AltAtual) {
    cQueda++;
  } else {
    cQueda = 0;
  }
  altApg = AltAtual;
  return cQueda >= VQueda;
}

//Paraquedas
int Paraquedas(int Pq, unsigned long TAt) {  //Numero do paraquedas (Pq, int), Tempo atual (TAt, Unsigned Long)

int ledst[Pq] = { LOW };

  if (lock[Pq] == 0)  //Trava para evitar que o paraquedas entre nesse loop de novo após ser ativado
  {
    if (q1 == 1 && ((sf[NF] <= -10) && (sf[NF] < hMin[Pq])) || !(sf[NF] <= -10))  //apg && (h && <altura certa> || !h) onde h é a condicional geral de usar a altura
    {                                                                                 //apg && ((sf[NF] <= -0.25) && (hMin[Pq] > sf[NF]) || !(sf[NF] <= 0.25))
      if (TAt > ta[Pq] && ta[Pq] != 0)  // Dessa forma TAt sempre é maior que os tempos dos paraquedas (começam em 0) (isso foi antes da adição de &&)
      {                                 // Condicional de tempo está bem ajustada pra 3/4 mas ainda não pra 1/2
        ledst[Pq] = HIGH;  //Acionar Paraquedas,
        tDes[Pq] = TAt + TL; //Registrar o tempo para desligar o paraquedas,
        lock[Pq] = 1; //Faz com que cada paraquedas entre nesse loop apenas uma vez
      }
    }
  }

  if (TAt > tDes[Pq])  // Tempo atual > Tempo de desligar?  -- desliga os paraquedas conforme necessário
  {
    ledst[Pq] = LOW;  //  Desligar o paraquedas
  }

  return ledst[Pq];
}
