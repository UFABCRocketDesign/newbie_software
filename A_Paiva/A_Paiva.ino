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
#define qf 2                      //Quantidade de filtros
#define NomeArq "apm"             //Nome do arquivo para o cartão SD entre aspas
#define espera 0000               //Tempo de espera para acionamento do paraquedas 2 (ms)
#define duracao 5000              //Tempo de duracao do acionamento dos paraquedas (ms)
#define altura 10                 //Altura para abertura do terceiro paraquedas

#define usa_bar 0                 //Variavel de escolha do uso do sensor BMP
#define usa_pre (usa_bar && 0)    //Variavel de escolha do uso do valor Pressão
#define usa_alt (usa_bar && 0)    //Variavel de escolha do uso do valor Altura
#define usa_altMax (usa_alt && 1) //Variavel de escolha do uso do valor Altura Máxima
#define usa_temp (usa_bar && 0)   //Variavel de escolha do uso do valor Temperatura
#define usa_apogeu (usa_bar && 0) //Variavel de escolha do uso da detecção de apogeu
#define usa_acpq (usa_apogeu && 1)//Variavel de escolha do uso do acionamento dos paraquedas

#define usa_giro 0                //Variavel de escolha do uso do sensor
#define usa_gx (usa_giro && 0)    //Variavel de escolha do uso do valor do giroscopio em x
#define usa_gy (usa_giro && 0)    //Variavel de escolha do uso do valor do giroscopio em y
#define usa_gz (usa_giro && 0)    //Variavel de escolha do uso do valor do giroscopio em z

#define usa_acel 0                //Variavel de escolha do uso de funções
#define usa_ax (usa_acel && 0)    //Variavel de escolha do uso do valor do acelerometro em x
#define usa_ay (usa_acel && 0)    //Variavel de escolha do uso do valor do acelerometro em y
#define usa_az (usa_acel && 0)    //Variavel de escolha do uso do valor do acelerometro em z

#define usa_mag 0                 //Variavel de escolha do uso de funções
#define usa_mx (usa_mag && 0)     //Variavel de escolha do uso do valor do magnetometro em x
#define usa_my (usa_mag && 0)     //Variavel de escolha do uso do valor do magnetometro em y
#define usa_mz (usa_mag && 0)     //Variavel de escolha do uso do valor do magnetometro em z

#define usa_Tempo 0              //Variavel de escolha do uso da impressão do tempo
#define usa_impreSerial 1         //Variavel de escolha do uso da impressão na serial
#define usa_SD 0                  //Variavel de escolha do uso de funções
//////////////////////////////////////////////////////////////////////
#define IGN_1 36  //act1 LED DA PLAQUINHA
#define IGN_2 61  //act2 LED DA PLAQUINHA
#define IGN_3 46  //act3 LED DA PLAQUINHA
#define IGN_4 55  //act4 LED DA PLAQUINHA

String cabecalho = ""; //ok
//#if usa_Tempo || usa_alt
//unsigned long tempoAtual = 0;     // will store last time LED was updated
//#endif
#if usa_apogeu || usa_alt
//float SomaRef = 0;                //Soma valores iniciais(foguete parado na base)
float AltitudeRef = 0;            //É o valor da média dos valores iniciais(foguete parado na base)ok
//float SomaMov = 0;                //Soma dos valores do vetor do filtro1
//float MediaMov = 0;               //É o valor da média dos valores do vetor do filtro1 ou altitude atual
//float MatrizFiltros[qf][tam];     //Vetor para guardar os valores para as médias utilizadas pelos filtros
#endif
#if usa_apogeu || usa_altMax
float Hmax = 0;                   //Valor máximo filtrado ok
#endif
#if usa_apogeu
//float Delta;                      //Diferença entre valor máximo do filtro1 (Hmax1) e valor atual referênciado (H11)
//int apogeu = 0;
#endif
#if usa_acpq
//int auxled1 = 0;
//int auxled2 = 0;
//int auxled3 = 0;
//unsigned long inicio1 = 0;        // will store last time LED was updated
//unsigned long inicio2 = 0;        // will store last time LED was updated
//unsigned long inicio3 = 0;        // will store last time LED was updated
//unsigned long inicio4 = 0;        // will store last time LED was updated
#endif
#if usa_temp
//float T;                          //Valor da Temperatura
#endif
#if usa_pre
//float P;                          //Valor da Pressão
//float Pm;                         //Valor da Pressão ao nivel do Mar
#endif
#if usa_bar
Adafruit_BMP085 bmp;              //Cria variável 'bmp' para a biblioteca Adafruit_BMP085
#endif
#if usa_giro
//int Gx;                           //Giroscópio em x
//int Gy;                           //Giroscópio em y
//int Gz;                           //Giroscópio em z
L3G giro;
#endif
#if usa_mag
//float Mx;                         //Magnetometro em x
//float My;                         //Magnetometro em y
//float Mz;                         //Magnetometro em z
Adafruit_HMC5883_Unified mag;// = Adafruit_HMC5883_Unified(12345);
#endif
#if usa_acel
//float Ax;                         //Acelerometro em x
//float Ay;                         //Acelerometro em y
//float Az;                         //Acelerometro em z
Adafruit_ADXL345_Unified accel;// = Adafruit_ADXL345_Unified(12345);
#endif
#if usa_SD
//int aux = 1;                      //Variavel auxiliar do while para criação de nome de arquivo do SD
//int tamNomeArq = 0;               //Valor da quantidade de caracteres da variavel NomeArq
//int Num = 0;                      //Valor da variavel que se somará e irá compor o nome do arquivo do SD
//int tamNum = 0;                   //Valor da quantidade de caracteres da variavel Num
//String x;                         //Primeira componente do nome do arquivo ou NomeArq
//String y;                         //Segunda componente do nome do arquivo ou preenchimento de zeros
//String z;                         //Terceira componente do nome do arquivo ou Num
String NomeFinal;                 //Nome final do arquivo do SD
//int sub1 = 0;                     //Variavel auxiliar para contagem de caracteres totais no nome do arquivo do SD;
//int sub2 = 0;                     //Variavel auxiliar para contagem de caracteres totais no nome do arquivo do SD;
const int chipSelect = 53;
#endif

void setup() {
  #if usa_acpq
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);//PINOS DA MACRO pinos.h feita pelo Heitor
  pinMode(IGN_2, OUTPUT);
  //pinMode(IGN_3, OUTPUT);
  //pinMode(IGN_4, OUTPUT);
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
  if(!mag.begin())
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
  if(!accel.begin())
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
  for (int i = 0; i < qf; i++) {
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
  unsigned long tempoAtual = millis();
  String dado = "";
  #if usa_Tempo
  dado += String(tempoAtual/1000.0);
  dado += "\t";
  #endif
  #if usa_giro
  giro.read();
  #if usa_gx
  int Gx = (int)giro.g.x;
  dado += String(Gx);
  dado += "\t";
  #endif
  #if usa_gy
  int Gy = (int)giro.g.y;
  dado += String(Gy);
  dado += "\t";
  #endif
  #if usa_gz
  int Gz = (int)giro.g.z;
  dado += String(Gz);
  dado += "\t";
  #endif
  #endif
  #if usa_mag
  sensors_event_t eventM; 
  mag.getEvent(&eventM);
  #if usa_mx
  float Mx = eventM.magnetic.x;
  dado += String(Mx);
  dado += "\t";
  #endif
  #if usa_my
  float My = eventM.magnetic.y;
  dado += String(My);
  dado += "\t";
  #endif
  #if usa_mz
  float Mz = eventM.magnetic.z;
  dado += String(Mz);
  dado += "\t";
  #endif
  #endif
  #if usa_acel
  sensors_event_t eventA; 
  accel.getEvent(&eventA);
  #if usa_ax
  float Ax = eventA.acceleration.x;
  dado += String(Ax);
  dado += "\t";
  #endif
  #if usa_ay
  float Ay = eventA.acceleration.y;
  dado += String(Ay);
  dado += "\t";
  #endif
  #if usa_az
  float Az = eventA.acceleration.z;
  dado += String(Az);
  dado += "\t";
  #endif
  #endif
  #if usa_temp
  float T = bmp.readTemperature();
  dado += String(T);
  dado += "\t";
  #endif
  #if usa_pre
  float P = bmp.readPressure();
  //Pm = bmp.readSealevelPressure();
  dado += String(P);
  dado += "\t";
  #endif
  #if usa_altMax  
  dado += String(Hmax);
  dado += "\t";
  #endif
  #if usa_apogeu || usa_alt
  float SomaMov = 0;                                         //Zera o SomaMov em todo loop
  float MatrizFiltros[qf][tam];
  for (int j = 0; j < qf; j++) {
    for (int i = tam - 2; i >= 0; i--) {                 //Esse 'for' anda com os valores do vetor do filtro1 de 1 em 1
      MatrizFiltros[j][i + 1] = MatrizFiltros[j][i];
    }
    if (j == 0) {
      MatrizFiltros[0][0] = bmp.readAltitude() - AltitudeRef; //Esse é o valor mais atualizado do filtro1
    }
    else {
      MatrizFiltros[j][0] = MediaMov;
    }
    SomaMov = 0;
    for (int i = 0; i <= tam - 1; i++) {                    //Esse 'for' faz a soma dos últimos valores medidos, para a média do filtro1
      SomaMov = SomaMov + MatrizFiltros[j][i];
    }
    float MediaMov = SomaMov / tam;
    #if usa_alt
    dado += String(MediaMov);                              //Printa a altura média de cada linha da matriz, ou seja, de cada filtro
    dado += "\t";
    #endif
  }
  #endif
  #if usa_apogeu || usa_altMax
  if (Hmax < MediaMov) {                                    //Pega o valor máximo da média/filtro2
    Hmax = MediaMov;
  }
  #endif
  #if usa_apogeu
  float Delta = Hmax - MediaMov;                                 //Compara o valor máximo do filtro1 com o valor atual do filtro1
  int apogeu = 0;
  unsigned long inicio1 = 0;
  unsigned long inicio2 = 0;
  if (Delta >= 2 && apogeu == 0) {                         //Quando a diferença de altitude for acima de 2 (metros), provavelmente o foguete está descendo ou pode haver um controle de quando se quer que abra o paraquedas
    apogeu = 1;
    #if usa_acpq
    unsigned long inicio1 = tempoAtual + duracao;
    unsigned long inicio2 = tempoAtual + espera;
    #endif
  }
  else if (apogeu == 0) {
    dado += "Subindo";
    dado += "\t";
  }
  if (apogeu == 1) {
    dado += "Descendo";
    dado += "\t";
  }
  #endif
  #if usa_acpq
  int auxled1 = 0;
  int auxled2 = 0;
  int auxled3 = 0;
  if (apogeu == 1) {
    if (auxled1 == 0) {
      digitalWrite(IGN_1, HIGH);
      auxled1 = 1;
      dado += "11";
    }
    if (tempoAtual >= inicio1 && auxled1 == 1) {
      digitalWrite(IGN_1, LOW);
      auxled1 = 2;
      dado += "01";
    }
    if (tempoAtual >= inicio2 && auxled2 == 0) {
      digitalWrite(IGN_2, HIGH);
      auxled2 = 1;
      unsigned long inicio3 = tempoAtual + duracao;
      dado += "12";
      //Serial.print("12");
    }
    if (tempoAtual >= inicio3 && auxled2 == 1) {
      digitalWrite(IGN_2, LOW);
      auxled2 = 2;
      dado += "02";
    }
    if (MediaMov <= altura && auxled3 == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      auxled3 = 1;
      unsigned long inicio4 = tempoAtual + duracao;
      dado += "13";
    }
    if (tempoAtual >= inicio4 && auxled3 == 1) {
      digitalWrite(LED_BUILTIN, LOW);
      auxled3 = 2;
      dado += "03";
    }
    dado += "\t";
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
