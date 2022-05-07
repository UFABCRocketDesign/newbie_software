#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <L3G.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//VALORES DE ENTRADA
#define tam 10                    //Tamanho da matriz do filtro(quantidade de valores usado)
#define qfa 3                      //Quantidade de filtros da altitude
#define qfg 3                      //Quantidade de filtros a mais para variaveis gerais
#define NomeArq "apm"             //Nome do arquivo para o cartão SD entre aspas
#define espera 1000               //Tempo de espera para acionamento do paraquedas 2 (ms)
#define duracao 2000              //Tempo de duracao do acionamento dos paraquedas (ms)
#define altura 10                 //Altura para abertura do terceiro paraquedas (m)

#define usa_bar 1                 //Variavel de escolha do uso do sensor BMP
#define usa_pre (usa_bar && 0)    //Variavel de escolha do uso do valor Pressão
#define usa_alt (usa_bar && 0)    //Variavel de escolha do uso do valor Altura
#define usa_altMax (usa_alt && 0) //Variavel de escolha do uso do valor Altura Máxima
#define usa_temp (usa_bar && 0)   //Variavel de escolha do uso do valor Temperatura
#define usa_apogeu (usa_bar && 1) //Variavel de escolha do uso da detecção de apogeu
#define usa_acpq (usa_apogeu && 1)//Variavel de escolha do uso do acionamento dos paraquedas

#define usa_giro 0                //Variavel de escolha do uso do sensor
#define usa_gx (usa_giro && 1)    //Variavel de escolha do uso do valor do giroscopio em x
#define usa_gy (usa_giro && 1)    //Variavel de escolha do uso do valor do giroscopio em y
#define usa_gz (usa_giro && 1)    //Variavel de escolha do uso do valor do giroscopio em z

#define usa_acel 0                //Variavel de escolha do uso de funções
#define usa_ax (usa_acel && 1)    //Variavel de escolha do uso do valor do acelerometro em x
#define usa_ay (usa_acel && 1)    //Variavel de escolha do uso do valor do acelerometro em y
#define usa_az (usa_acel && 1)    //Variavel de escolha do uso do valor do acelerometro em z

#define usa_mag 0                 //Variavel de escolha do uso de funções
#define usa_mx (usa_mag && 1)     //Variavel de escolha do uso do valor do magnetometro em x
#define usa_my (usa_mag && 1)     //Variavel de escolha do uso do valor do magnetometro em y
#define usa_mz (usa_mag && 1)     //Variavel de escolha do uso do valor do magnetometro em z

#define usa_Tempo 1               //Variavel de escolha do uso da impressão do tempo
#define usa_impreSerial 1         //Variavel de escolha do uso da impressão na serial
#define usa_SD 0                  //Variavel de escolha do uso de funções
//////////////////////////////////////////////////////////////////////
#define IGN_1 36  //act1 LED DA PLAQUINHA
#define IGN_2 61  //act2 LED DA PLAQUINHA
#define IGN_3 46  //act3 LED DA PLAQUINHA
#define IGN_4 55  //act4 LED DA PLAQUINHA

String dado = "";
String cabecalho = "";
#if usa_apogeu || usa_alt
float AltitudeRef = 0;            //É o valor da média dos valores iniciais(foguete parado na base)
float MatrizFiltros[qfa + qfg][tam];   //Vetor para guardar os valores para as médias utilizadas pelos filtros
#endif
#if usa_apogeu || usa_altMax
float Hmax = 0;                   //Valor máximo filtrado
#endif
#if usa_apogeu
int apogeu = 0;
#endif
#if usa_acpq
//int auxled1 = 0;
//int auxled2 = 0;
//int auxled3 = 0;
//int auxinicio1 = 0;
//int auxinicio2 = 0;
//unsigned long inicio1 = 0;        // will store last time LED was updated
//unsigned long inicio2 = 0;        // will store last time LED was updated
//unsigned long inicio3 = 0;        // will store last time LED was updated
//unsigned long inicio4 = 0;        // will store last time LED was updated
#endif
#if usa_bar
Adafruit_BMP085 bmp;              //Cria variável 'bmp' para a biblioteca Adafruit_BMP085
#endif
#if usa_giro
L3G giro;
#endif
#if usa_mag
Adafruit_HMC5883_Unified mag;// = Adafruit_HMC5883_Unified(12345);
#endif
#if usa_acel
Adafruit_ADXL345_Unified accel;// = Adafruit_ADXL345_Unified(12345);
#endif
#if usa_SD
String NomeFinal;                 //Nome final do arquivo do SD
const int chipSelect = 53;
#endif
///////// classes ou objetos ///////////

class Filtro {
  private:
    int const QtTermos;
    float* const VetorFiltro;  //ponteiro como vetor
    float MediaMov;
  public:
    //construtor
    Filtro(int v_QtTermos) : QtTermos(v_QtTermos), VetorFiltro(new float[QtTermos]) {  //"new" pega o valor para tamanho do vetor
      for (int i = 0; i < QtTermos; i++) {
        VetorFiltro[i] = 0;
      }
    }
    //destrutor
    ~Filtro() {
      delete[] VetorFiltro;
    }
    float FuncaoFriutu(float valoratualizado);
};
float Filtro::FuncaoFriutu(float valoratualizado) {
  float SomaMov = 0;                                 //Declara e zera o SomaMov em todo loop
  MediaMov = 0;                                      //Zera o MediaMov em todo loop
  int QtTermosm2 = QtTermos - 2;
  for (int i = QtTermosm2; i >= 0; i--) {    //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1 de trás pra frente
    VetorFiltro[i + 1] = VetorFiltro[i];
  }
  VetorFiltro[0] = valoratualizado;                  //Esse é o valor mais atualizado do filtro1
  for (int i = 0; i < QtTermos; i++) {    //Esse 'for' faz a soma dos valores da matriz, para a média do filtro1
    SomaMov = SomaMov + VetorFiltro[i];
  }
  MediaMov = SomaMov / QtTermos;               //Valor final do filtro, uma média entre "tam" quantidades de valores
  return MediaMov;
}

class CascataDeFiltro {
  private:
    int const QtTermos;
    int const QtFiltros;
    Filtro** MatrizFiltro;
  public:
    //construtor
    CascataDeFiltro(int v_QtFiltros, int v_QtTermos) : QtFiltros(v_QtFiltros), QtTermos(v_QtTermos), MatrizFiltro(new Filtro * [QtFiltros]) {
      for (int i = 0; i < QtFiltros; i++) {
        MatrizFiltro[i] = new Filtro(QtTermos);
      }
    }  //"new" pega o valor para tamanho do vetor de vetor
    //destrutor
    ~CascataDeFiltro() {
      for (int i = 0; i < QtFiltros; i++) {
        delete MatrizFiltro[i];
      }
      delete[] MatrizFiltro;
    }
    float FuncaoCascataFriutu(float valoratualizado);
};
float CascataDeFiltro::FuncaoCascataFriutu(float valoratualizado) {
  float ValorFiltrado = valoratualizado;
  for (int i = 0; i < QtFiltros; i++) {
    ValorFiltrado = MatrizFiltro[i]->FuncaoFriutu(ValorFiltrado);      //o "->" chama uma função para ponteiro de ponteiro
    dado += String(ValorFiltrado) + "\t";                            //Printa a altura média de cada linha da matriz, ou seja, de cada filtro
    //Serial.println(ValorFiltrado);
  }
  return ValorFiltrado;
}

class Paraquedas {
  private:
    static int Apogeu;
    static unsigned long TempoAtual;
    static float AlturaAtual;
    //tipo(1°por tempo a partir do apogeu, 2°em uma altura específica)
    int Tipo;
    int Pino;
    //duração do acionamentp
    float Duracao;
    //espera para ser acionado
    float Espera;
    //altura específica para acionar após apogeu
    float Alturaesp;
    int auxled = 0;
    int auxespera = 0;
    unsigned long inicio = 0;
    unsigned long fim = 0;
  public:
    //construtor
    Paraquedas(int v_tipo, int v_pino, float v_duracao, float v_espera, float v_alturaesp): Tipo(v_tipo), Pino(v_pino), Duracao(v_duracao), Espera(v_espera), Alturaesp(v_alturaesp) {}
    //usar "static" para variavel compartilhada entre os objetos dessa classe (tempo atual, detecção de apogeu, altura atual)
    //ao usar a varial static, a função que mexe com ela tem que ser static tbm, e o chamamento da função deve ser através da class e não de um objeto especifico "nomeclasse::nomefunção()"
    //definir os pinos de acionamento, porém iniciar o pinMode em uma função chamada no setup, pois o construtor roda primeiro e pode dar ruim
    static void AtualizaApogeu(int v_Apogeu) {
      Apogeu = v_Apogeu;
    }
    static void AtualizaTempoAtual(unsigned long v_TempoAtual) {
      TempoAtual = v_TempoAtual;
    }
    static void AtualizaAlturaAtual(float v_MediaMov) {
      AlturaAtual = v_MediaMov;
    }
    void declaraPino() {
      int auxpino = 0;
      if (auxpino == 0) {
        pinMode(Pino, OUTPUT);
        auxpino = 1;
      }
    }
    String Paraqueda() {
      if (Tipo == 1) {
        if (Apogeu == 1) {
          if (auxespera == 0) {
            inicio = TempoAtual + Espera;
            auxespera = 1;
          }
          if (TempoAtual >= inicio && auxled == 0) {
            digitalWrite(Pino, HIGH);
            auxled = 1;
            fim = TempoAtual + Duracao;
            return "acionado";
            //Serial.print("12");
          }
          if (TempoAtual >= fim && auxled == 1) {
            digitalWrite(Pino, LOW);
            auxled = 2;
            return "desacionado";
          }
          return "";
        }
      }
      else if (Tipo == 2) {
        if (Apogeu == 1) {
          if (AlturaAtual <= Alturaesp && auxled == 0) {
            digitalWrite(Pino, HIGH);
            auxled = 1;
            fim = TempoAtual + Duracao;
            return "acionado";
          }
          if (TempoAtual >= fim && auxled == 1) {
            digitalWrite(Pino, LOW);
            auxled = 2;
            return "desacionado";
          }
          return "";
        }
      }
    }
};
int Paraquedas::Apogeu=0;
unsigned long Paraquedas::TempoAtual=0;
float Paraquedas::AlturaAtual=0;

Filtro FiltroAx(tam);
Filtro FiltroAy(tam);
Filtro FiltroAz(tam);
Filtro FiltroAltitude(tam);
CascataDeFiltro CascataFiltroAltitude(qfa, tam);
Paraquedas Parachute1(1, IGN_1, duracao, 0, altura);
Paraquedas Parachute2(1, IGN_2, duracao, espera, altura);
Paraquedas Parachute3(3, IGN_3, duracao, espera, altura);

void setup() {
#if usa_acpq
  Parachute1.declaraPino();
  Parachute2.declaraPino();
  Parachute3.declaraPino();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);//PINOS DA MACRO pinos.h feita pelo Heitor
  pinMode(IGN_2, OUTPUT);
  //pinMode(IGN_3, OUTPUT);
  //pinMode(IGN_4, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(IGN_1, LOW);
  digitalWrite(IGN_2, LOW);
  //digitalWrite(IGN_3, LOW);
  //digitalWrite(IGN_4, LOW);
#endif
#if usa_impreSerial
  Serial.begin(115200);
#endif
  Wire.begin();
#if usa_Tempo
  cabecalho = "Tempo\t";
#endif
#if usa_giro
  if (!giro.init()) {
    Serial.println("FALHA AO ENCONTRAR GIROSCÓPIO!");
  }
  giro.enableDefault();
#endif
#if usa_gx
  cabecalho += "Gx\t";
#endif
#if usa_gy
  cabecalho += "Gy\t";
#endif
#if usa_gz
  cabecalho += "Gz\t";
#endif
#if usa_mag
  if (!mag.begin())
  {
    Serial.println("FALHA AO ENCONTRAR MAGNETÔMETRO!");
  }
#endif
#if usa_mx
  cabecalho += "Mx(uT)\t";
#endif
#if usa_my
  cabecalho += "My(uT)\t";
#endif
#if usa_mz
  cabecalho += "Mz(uT)\t";
#endif
#if usa_acel
  if (!accel.begin())
  {
    Serial.println("FALHA AO ENCONTRAR ACELERÔMETRO");
  }
  accel.setRange(ADXL345_RANGE_16_G);
#endif
#if usa_ax
  cabecalho += "Ax(m/s^2)\t";
#endif
#if usa_ay
  cabecalho += "Ay(m/s^2)\t";
#endif
#if usa_az
  cabecalho += "Az(m/s^2)\t";
#endif
#if usa_bar
  if (!bmp.begin()) {
    Serial.println("FALHA AO ENCONTRAR BARÔMETRO");
  }
#endif
#if usa_temp
  cabecalho += "Temperatura(°C)\t";
#endif
#if usa_pre
  cabecalho += "Pressao(Pa)\t";
#endif
#if usa_altMax
  cabecalho += "Altura máxima(m)\t";
#endif
#if usa_alt
  for (int i = 0; i < qfa; i++) {
    cabecalho += "Altura do filtro ";
    cabecalho += i;
    cabecalho += "(m)\t";
  }
#endif
#if usa_acpq || usa_apogeu
  cabecalho += "Statu de voo";
#endif
#if usa_alt || usa_apogeu
  float SomaRef = 0;
  for (int i = 0; i < 100; i++) {                       //Este 'for' serve para definir a altitude da base de lançamento como valor de referência.
    SomaRef = SomaRef + bmp.readAltitude();
  }
  AltitudeRef = SomaRef / 100;
#endif
#if usa_impreSerial
  Serial.println("Dados de altitude de voo");
  Serial.println(cabecalho);
#endif
#if usa_SD
  Serial.println("Inicializando cartão SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("FALHA NO CARTÃO SD");
  }
  String x = NomeArq;
  int tamNomeArq = x.length();
  int sub1 = 8 - tamNomeArq;
  int aux = 1;
  while (aux == 1) {
    String z = String(Num);
    int tamNum = z.length();
    int sub2 = sub1 - tamNum;
    String y = "";
    for (int i = 0; i < sub2; i++) {
      y = y + "0";
    }
    NomeFinal = x + y + z + ".txt";
    if (SD.exists(NomeFinal)) {
      //Serial.print(NomeFinal);
      //Serial.println(" ja existe");
      int Num++;
      aux = 1;
    }
    else {
      Serial.print("Nome do arquivo atual: ");
      Serial.println(NomeFinal);
      aux = 0;
    }
  }
  File dataFile = SD.open(NomeFinal, FILE_WRITE);
  if (dataFile) {
    dataFile.println(cabecalho);
    dataFile.close();
  }
#endif
}
void loop() {
  dado = "";
  unsigned long tempoAtual = millis();
  Paraquedas::AtualizaTempoAtual(tempoAtual);
#if usa_Tempo
  dado += String(tempoAtual / 1000.0) + "\t";
#endif
#if usa_giro
  giro.read();
#if usa_gx
  int Gx = giro.g.x;
  dado += String(Gx) + "\t";
#endif
#if usa_gy
  int Gy = giro.g.y;
  dado += String(Gy) + "\t";
#endif
#if usa_gz
  int Gz = giro.g.z;
  dado += String(Gz) + "\t";
#endif
#endif
#if usa_mag
  sensors_event_t eventM;
  mag.getEvent(&eventM);
#if usa_mx
  float Mx = eventM.magnetic.x;
  dado += String(Mx) + "\t";
#endif
#if usa_my
  float My = eventM.magnetic.y;
  dado += String(My) + "\t";
#endif
#if usa_mz
  float Mz = eventM.magnetic.z;
  dado += String(Mz) + "\t";
#endif
#endif
#if usa_acel
  sensors_event_t eventA;
  accel.getEvent(&eventA);
#if usa_ax
  float Ax = eventA.acceleration.x;
  dado += String(Ax) + "\t";
  //float Axfiltrada = Friutu(Ax, qfa+1);
  float Axfiltrada = FiltroAx.FuncaoFriutu(Ax);
  dado += String(Axfiltrada) + "\t";
#endif
#if usa_ay
  float Ay = eventA.acceleration.y;
  dado += String(Ay) + "\t";
  //float Ayfiltrada = Friutu(Ay, qfa+2);
  float Ayfiltrada = FiltroAy.FuncaoFriutu(Ay);
  dado += String(Ayfiltrada) + "\t";
#endif
#if usa_az
  float Az = eventA.acceleration.z;
  dado += String(Az) + "\t";
  //float Azfiltrada1 = Friutu(Az, qfa+3);
  float Azfiltrada = FiltroAz.FuncaoFriutu(Az);
  dado += String(Azfiltrada) + "\t";
#endif
#endif
#if usa_temp
  float T = bmp.readTemperature();
  dado += String(T) + "\t";
#endif
#if usa_pre
  float P = bmp.readPressure();
  //float Pm = bmp.readSealevelPressure();
  dado += String(P) + "\t";
#endif
#if usa_alt
  //float A = bmp.readAltitude() - AltitudeRef;
  dado += String(A) + "\t";
#endif
#if usa_altMax
  dado += String(Hmax) + "\t";
#endif
#if usa_apogeu || usa_alt
  float A = bmp.readAltitude() - AltitudeRef;
  //  float Afiltrada = A;                                     //Chama a função de filtro para a altitude
  //  for(int i = 0; i < qfa; i++){
  //    Afiltrada = Friutu(Afiltrada, i);
  //    dado += String(Afiltrada)+"\t";                              //Printa a altura média de cada linha da matriz, ou seja, de cada filtro
  //  }
  float Afiltrada = CascataFiltroAltitude.FuncaoCascataFriutu(A);
  Paraquedas::AtualizaAlturaAtual(Afiltrada);
  dado += String(Afiltrada) + "\t";
#endif
  //Serial.print("Meio loop,dps dos filtros");
#if usa_apogeu || usa_altMax
  if (Hmax < Afiltrada) {                                    //Pega o valor máximo da média/filtro2
    Hmax = Afiltrada;
  }
#endif
#if usa_apogeu
  apogeu = Apogueu(apogeu, Hmax, Afiltrada, tempoAtual);
  Paraquedas::AtualizaApogeu(apogeu);
  if (apogeu == 0) {
    dado += "Subindo\t";
  }
  else if (apogeu == 1) {
    dado += "Descendo\t";
#if usa_acpq
    dado += Parachute1.Paraqueda();
    dado += "\t";
    dado += Parachute2.Paraqueda();
    dado += "\t";
//    dado += Parachute3.Paraqueda();
//    dado += Paraqueda2(tempoAtual, apogeu);
//    dado += Paraqueda3(tempoAtual, Afiltrada, apogeu);
    dado += "\t";
#endif
  }
#endif
#if usa_SD
  File dataFile = SD.open(NomeFinal, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dado);
    dataFile.close();
  }
#endif
#if usa_impreSerial
  Serial.println(dado);
#endif
}

////funçoes////

//float Friutu(float valoratualizado, int j) {
//  float SomaMov = 0;                                 //Declara e zera o SomaMov em todo loop
//  float MediaMov = 0;                                //Declara e zera o MediaMov em todo loop
//  for (int i = tam - 2; i >= 0; i--) {               //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1 de trás pra frente
//    MatrizFiltros[j][i + 1] = MatrizFiltros[j][i];
//  }
//  MatrizFiltros[j][0] = valoratualizado;             //Esse é o valor mais atualizado do filtro1
//  for (int i = 0; i <= tam - 1; i++) {               //Esse 'for' faz a soma dos valores da matriz, para a média do filtro1
//    SomaMov = SomaMov + MatrizFiltros[j][i];
//  }
//  MediaMov = SomaMov / tam;                          //Valor final do filtro, uma média entre "tam" quantidades de valores
//  return MediaMov;
//}
//Paraquedas::AtualizaAlturaAtual(MediaMov);
int Apogueu(int apogeu, float Hmax, float MediaMov, unsigned long tempoAtual) {
  float Delta = Hmax - MediaMov;                           //Compara o valor máximo do filtro1 com o valor atual do filtro1
  if (Delta >= 2 && apogeu == 0) {                         //Quando a diferença de altitude for acima de 2 (metros), provavelmente o foguete está descendo ou pode haver um controle de quando se quer que abra o paraquedas
    apogeu = 1;
  }
  return apogeu;
}
//Paraquedas::AtualizaApogeu(apogeu);
/*#if usa_acpq
String Paraqueda1(unsigned long tempoAtual, int apogeu) {
  if (auxinicio1 == 0) {
    inicio1 = tempoAtual + duracao;
    auxinicio1 = 1;
  }
  if (apogeu == 1) {
    if (auxled1 == 0) {
      digitalWrite(IGN_1, HIGH);
      auxled1 = 1;
      return "11";
    }
    if (tempoAtual >= inicio1 && auxled1 == 1) {
      digitalWrite(IGN_1, LOW);
      auxled1 = 2;
      return "01";
    }
  }
  return "";
}
String Paraqueda2(unsigned long tempoAtual, int apogeu) {
  if (apogeu == 1) {
    if (auxinicio2 == 0) {
      inicio2 = tempoAtual + espera;
      auxinicio2 = 1;
    }
    if (tempoAtual >= inicio2 && auxled2 == 0) {
      digitalWrite(IGN_2, HIGH);
      auxled2 = 1;
      inicio3 = tempoAtual + duracao;
      return "12";
      //Serial.print("12");
    }
    if (tempoAtual >= inicio3 && auxled2 == 1) {
      digitalWrite(IGN_2, LOW);
      auxled2 = 2;
      return "02";
    }
    return "";
  }
}
String Paraqueda3(unsigned long tempoAtual, float MediaMov, int apogeu) {
  if (apogeu == 1) {
    if (MediaMov <= altura && auxled3 == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      auxled3 = 1;
      inicio4 = tempoAtual + duracao;
      return "13";
    }
    if (tempoAtual >= inicio4 && auxled3 == 1) {
      digitalWrite(LED_BUILTIN, LOW);
      auxled3 = 2;
      return "03";
    }
    return "";
  }
}
#endif*/
