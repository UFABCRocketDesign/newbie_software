#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <L3G.h>
L3G giroscopio;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
Adafruit_BMP085 bmp;

//#define IGN_1 36  /*act1*/
//#define IGN_2 61  /*act2*/
//#define IGN_3 46  /*act3*/ 
//#define IGN_4 55  /*act4*/

#define EXIST_SD 0 
#define EXIST_GIRO 1
#define EXIST_ACEl 0
#define EXIST_MAG 0
#define EXIST_IGN_1 1
#define EXIST_IGN_2 1
#define EXIST_IGN_3 1

#define chipSelect 53
#define led_piloto 36
#define led_secundario 61
#define led_final 13

#define pmt 20 // intervalo de valores usados na para media movel
#define nf 3  // Numero de filtros 
#define ncp 4  // intervalo de comparação para queda e suibida

#define intervalo 5000 // intervalo de tempo para acionamento dos paraquedas


#if EXIST_SD
 int numero_do_SD;  //contem o numero que vai seguir o nome do sd
 int qnt_zero;  //quantidade de zeros que será adicionado no nome do sd
 String nome_SD; //primeiro nome do sd
 String txt_SD;
 String zeros;
 String Projeto_name; //Nome compketo do SD
 String complemento_SD;  // contem o numero do sd em string
#endif

#if EXIST_GIRO
 String estado_giroscopio;
#endif

#if EXIST_ACEl
 String estado_acelerometro;
#endif

#if EXIST_MAG
 String estado_sensormag;
#endif

#if EXIST_IGN_1
 int acendeu_piloto;  // contem o estado do led piloto, 1 para acesso e 0 para apagado
 bool Trava_piloto = true; //trava usado para o led piloto
#endif

#if EXIST_IGN_2
 int acendeu_secundario;  // contem o estado do led secundario, 1 para acesso e 0 para apagado
 unsigned long Time_secundario = 0;
 bool Trava_secundario = true; //trava usado para o led secundario
#endif

#if EXIST_IGN_3
 int acendeu_final; // contem o estado do led final, 1 para acesso e 0 para apagado
 unsigned long Time_final = 0;
 bool Trava_final = true; //trava usado para o led secundario
#endif

int var_queda;  // contem o estado do foguete, 1 para subindo e 0 para queda

float z;
float ref_chao; // constante que será usada para referenciar o chão
float vetor[nf][pmt]; // movimentaçãop dos filtros de sinal de alrura 
float sinal[nf+1];   // irá conter todos sinais relacionado a altura  
float sinalzin[ncp]; // contem os dados usados para comparar a altura 
float tempo_atual;

unsigned long Time_do_apogeu = 0;

bool Trava_apogeu = true; //trava usado para o led piloto

String dados_string = ""; // irá conter os dados dos sinais em string 



//----------------------------------------------------------------------------------
void setup() {
  #if EXIST_IGN_1
   pinMode(led_piloto, OUTPUT);
   digitalWrite(led_piloto, LOW);
  #endif
  #if EXIST_IGN_2
   pinMode(led_secundario, OUTPUT);
   digitalWrite(led_secundario, LOW);
  #endif
  #if EXIST_IGN_3
   pinMode(led_final, OUTPUT);
   digitalWrite(led_final, LOW);
  #endif
  
  Serial.begin(115200);
  Wire.begin();

  if (!bmp.begin()){Serial.println("Could not find a valid BMP085 sensor, check wiring!");}

  #if EXIST_GIRO
   if (!giroscopio.init()){Serial.println("Failed to autodetect gyro type!");}
   giroscopio.enableDefault();
  #endif

  #if EXIST_ACEl
   if(!accel.begin()){Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");}
   accel.setRange(ADXL345_RANGE_16_G);
  #endif

  #if EXIST_MAG
   if(!mag.begin()){Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");}
  #endif

  // calcula a media dos dados para referenciar a altura
  call_chao(); 

   dados_string = "";
   dados_string += "Tempo atual(s):\t";
   dados_string += "Alt(m):\t";
   dados_string += "Detecção de queda:\t";
   #if EXIST_IGN_1
   dados_string += "Estado do led piloto:\t";
   #endif
   #if EXIST_IGN_2
    dados_string +="Estado do led secundario:\t";
   #endif
   #if EXIST_IGN_3
    dados_string +="Estado do led Final:\t";
   #endif
   #if EXIST_GIRO
    dados_string +="Giroscopio em X:\t";
    dados_string +="Giroscopio em Y:\t";
    dados_string +="Giroscopio em Z:\t";
   #endif
   #if EXIST_ACEl
    dados_string +="Acelerometro em X(m/s^2):\t";
    dados_string +="Acelerometro em Y(m/s^2):\t";
    dados_string +="Acelerometro em Z(m/s^2):\t";
   #endif
   #if EXIST_MAG
    dados_string +="Sensormag em X(uT):\t";
    dados_string +="Sensormag em Y(uT):\t";
    dados_string +="Sensormag em Z(uT):\t";
   #endif
   dados_string +="Constante que referencia o chão = ";
   dados_string +=(String(ref_chao));

   
  // laço para determinar o nome do SD
  #if EXIST_SD
   if (!SD.begin(chipSelect)){Serial.println("Card failed, or not present");}
   numero_do_SD = 0;
   nome_SD = "Kob_";
   txt_SD = ".txt";
   do{
     zeros = "";
     Projeto_name = "";
     complemento_SD = String(numero_do_SD);
     qnt_zero = 8 - nome_SD.length() - complemento_SD.length();
     for (int i = 0; i < qnt_zero; i++) {
       zeros += "0";  
     }
      Projeto_name = nome_SD + zeros + complemento_SD + txt_SD;
      numero_do_SD ++;
   }while(SD.exists(Projeto_name));
   Serial.println(Projeto_name);
   
   File dataFile = SD.open(Projeto_name, FILE_WRITE);
    if(dataFile){
       dataFile.println(dados_string);
       dataFile.close();
    }else {
      Serial.println("Erro ao abrir o SD");
    } 
   dataFile.close();
  #endif
  
  Serial.print(dados_string);
  Serial.println(); 

  

}
//----------------------------------------------------------------------------------
void loop() {
  
  // Serial.print(bmp.readTemperature());
  // Serial.print("\t");
  // Serial.print(bmp.readPressure());
  // Serial.print("\t");

  sinal[0] = bmp.readAltitude() - ref_chao;
  tempo_atual = millis();
  
  Filtros();
  Detec_queda();
  #if EXIST_IGN_1
   Led_para_queda_piloto();
  #endif
  #if EXIST_IGN_2
   Led_para_queda_secundario();
  #endif
  #if EXIST_IGN_3
   Led_para_queda_final();
  #endif
  #if EXIST_GIRO
   Giroscopio();
  #endif
  #if EXIST_ACEl
   Acelerometro();
  #endif
  #if EXIST_MAG
   Magsensor();
  #endif
  Salvar();

  Serial.print(dados_string);
  Serial.println();
}

//-----------------------------------------------------------------------------
void call_chao() {
  z = 0;
  for (int x = 0; x < 100; x++) {
    z = bmp.readAltitude() + z;
  }
  ref_chao = z / 100; 
}
//----------------------------------------------------------------------------------
void Filtros() {
  for (int y = 0; y < nf; y++) {
    for (int x = pmt - 1; x > 0; x--) {
      vetor[y][x] = vetor[y][x - 1];
    }
    vetor[y][0] = sinal[y];
    float k = 0;
    for (int x = 0; x < pmt; x++) {
      k = vetor[y][x] + k;
    }
    sinal[y + 1] = k / pmt;
  }
}

//----------------------------------------------------------------------------------
void Detec_queda() {
  for (int x = ncp - 1; x > 0; x--) {
    sinalzin[x] = sinalzin[x - 1];
  }
  sinalzin[0] = sinal[nf];
  if ( sinalzin[0] > sinalzin[ncp - 1]){
    var_queda = 1;
  }else{
    var_queda = 0;
    // define o a hora do ainicio do apogeu
    if(Trava_apogeu == true){
      Time_do_apogeu = tempo_atual;
      Trava_apogeu = false;
    }
  }
}
//----------------------------------------------------------------------------------

#if EXIST_IGN_1
void Led_para_queda_piloto() {
    if (var_queda == 0){
     if(Trava_piloto == true){
       digitalWrite(led_piloto, HIGH);
       acendeu_piloto = 1;
       Trava_piloto = false;
     }
     if(Trava_piloto == false){
       if ((tempo_atual - Time_do_apogeu) >= intervalo) {
         digitalWrite(led_piloto, LOW);
         acendeu_piloto = 0;
       }
     }
   }
 }
#endif

//----------------------------------------------------------------------------------

#if EXIST_IGN_2
 void Led_para_queda_secundario() {
   if (var_queda == 0){
     if(Trava_secundario == true){
       if ((tempo_atual - Time_do_apogeu) >= intervalo){
         digitalWrite(led_secundario, HIGH);
         Time_secundario = tempo_atual;
         acendeu_secundario = 1;
         Trava_secundario = false;
       }
     }
     if(Trava_secundario == false){
       if ((tempo_atual - Time_secundario) >= intervalo) {
         digitalWrite(led_secundario, LOW);
         acendeu_secundario = 0;
       }
     }
   }
 }
#endif

//----------------------------------------------------------------------------------

#if EXIST_IGN_3
 void Led_para_queda_final() {
   if (var_queda == 0){
     if(sinal[nf] <= 400){
       if(Trava_final == true){
          digitalWrite(led_final, HIGH);
          Time_final = tempo_atual;
          acendeu_final = 1;
          Trava_final = false;
        }
        if(Trava_final == false){
          if((tempo_atual - Time_final) >= intervalo) {
            digitalWrite(led_final, LOW);
            acendeu_final = 0;
         }
       }
     }
   }
 }
#endif

//----------------------------------------------------------------------------------

#if EXIST_GIRO
 void Giroscopio(){
   giroscopio.read();
   estado_giroscopio = "";
   estado_giroscopio += giroscopio.g.x;
   estado_giroscopio += "\t";
   estado_giroscopio += giroscopio.g.y;
   estado_giroscopio += "\t";
   estado_giroscopio += giroscopio.g.z;
 }
#endif

//----------------------------------------------------------------------------------

#if EXIST_ACEl
 void Acelerometro(){
   sensors_event_t event; 
   accel.getEvent(&event);
   estado_acelerometro = "";
   estado_acelerometro += String(event.acceleration.x);
   estado_acelerometro += "\t";
   estado_acelerometro += String(event.acceleration.y);
   estado_acelerometro += "\t";
   estado_acelerometro += String(event.acceleration.z);
 }
#endif

//----------------------------------------------------------------------------------

#if EXIST_MAG
 void Magsensor(){
   sensors_event_t event; 
   mag.getEvent(&event);
   estado_sensormag = "";
   estado_sensormag += String(event.magnetic.x);
   estado_sensormag += "\t";
   estado_sensormag += String(event.magnetic.y);
   estado_sensormag += "\t";
   estado_sensormag += String(event.magnetic.z);
}
#endif

//----------------------------------------------------------------------------------

void Salvar(){
  dados_string = "";
  dados_string += String(tempo_atual/1000);
  dados_string += "\t";
  dados_string += String(sinal[nf]);
  dados_string += "\t";
  dados_string += String(var_queda);
  #if EXIST_IGN_1
   dados_string += "\t";
   dados_string += String(acendeu_piloto);
  #endif
  #if EXIST_IGN_2
   dados_string += "\t";
   dados_string += String(acendeu_secundario);
  #endif
  #if EXIST_IGN_3
   dados_string += "\t";
   dados_string += String(acendeu_final);
  #endif
  #if EXIST_GIRO
   dados_string += "\t";
   dados_string += String(estado_giroscopio);
  #endif
  #if EXIST_ACEl
   dados_string += "\t";
   dados_string += String(estado_acelerometro);
  #endif
  #if EXIST_MAG
   dados_string += "\t";
   dados_string += String(estado_sensormag);
  #endif
  #if EXIST_SD 
   File dataFile = SD.open(Projeto_name, FILE_WRITE);
   if(dataFile){
      dataFile.println(dados_string);
      dataFile.close();
   }else {
     Serial.println("Erro ao abrir o SD");
   } 
  #endif
}

//----------------------------------------------------------------------------------
