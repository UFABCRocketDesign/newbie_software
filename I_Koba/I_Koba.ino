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

#define EXIST_TEMPO 0
#define EXIST_SD 0
#define EXIST_BAR 1
#define EXIST_GIRO 0
#define EXIST_ACEl 0
#define EXIST_MAG 0

#define EXIST_IGN_1 (EXIST_BAR && 1)
#define EXIST_IGN_2 (EXIST_BAR && 1)
#define EXIST_IGN_3 (EXIST_BAR && 1)

#define EXIST_GIRO_X (EXIST_GIRO && 1)
#define EXIST_GIRO_Y (EXIST_GIRO && 1)
#define EXIST_GIRO_Z (EXIST_GIRO && 1)

#define EXIST_ACEl_X (EXIST_ACEl && 1)
#define EXIST_ACEl_Y (EXIST_ACEl && 1)
#define EXIST_ACEl_Z (EXIST_ACEl && 1)

#define EXIST_MAG_X (EXIST_MAG && 1)
#define EXIST_MAG_Y (EXIST_MAG && 1)
#define EXIST_MAG_Z (EXIST_MAG && 1)

#define EXIST_ALTURA (EXIST_BAR && 1)

#define chipSelect 53
#define IGN_1 36 //paraquedas piloto
#define IGN_2 61 //paraquedas secundario
#define led_final 13 //paraquedas final

#define MEDIA_PARA_CHAO 100 // intervalo de valores usados na para media do calculo do chão
#define INTERVALO_MEDIA_M 20 // intervalo de valores usados na para media movel
#define SOMA_FINAL_QUEDA 12 // Numero define o valor final para queda 
#define NUMERO_FILTROS 3  // Numero de filtros 
#define ALTURA_DE_ATIVACAO 400 // ALTURA minima para ativar o ultimo paraquedas

#define TEMPO_PILOTO_ON 5000 // intervalo de tempo que o paraquedas piloto vai ficar ligado
#define TEMPO_ATIVAR_SECUNDARIO 8000 // intervalo de tempo necessario para ativar o paraquedas secundario pos inicio de queda
#define TEMPO_SECUNDARIO_ON 5000 // intervalo de tempo que o paraquedas secundariio vai ficar ligado
#define TEMPO_FINAL_ON 5000 // intervalo de tempo que o paraquedas final vai ficar ligado




#if EXIST_SD
String projeto_name; //Nome compketo do SD
#endif //SD

#if EXIST_GIRO
String estado_giroscopio;
#endif //giroscopiio 

#if EXIST_ACEl
String estado_acelerometro;
#endif //acelerometro

#if EXIST_MAG
String estado_sensormag;
#endif //sensor campo magnetico

#if EXIST_IGN_1
int acendeu_piloto;  // contem o estado do led piloto, 1 para acesso e 0 para apagado
bool trava_piloto = true; //trava usado para o led piloto
#endif //paraquedas piloto

#if EXIST_IGN_2
int acendeu_secundario;  // contem o estado do led secundario, 1 para acesso e 0 para apagado
bool trava_secundario = true; //trava usado para o led secundario
unsigned long time_secundario = 0;
#endif //paraquedas secundario

#if EXIST_IGN_3
int acendeu_final; // contem o estado do led final, 1 para acesso e 0 para apagado
bool trava_final = true; //trava usado para o led secundario
unsigned long time_final = 0;
#endif //paraquedas final

#if EXIST_BAR
boolean var_queda;  // contem o estado do foguete, 1 para subindo e 0 para queda
float ref_chao; // constante que será usada para referenciar o chão
int arm_queda;
float vetor[NUMERO_FILTROS+1][INTERVALO_MEDIA_M]; // movimentaçãop dos filtros de sinal de alrura
float sinal[NUMERO_FILTROS + 1]; // irá conter todos sinais relacionado a altura
float sinalzin;
unsigned long time_do_apogeu = 0;
bool trava_apogeu = true; //trava usado para o led piloto
#endif //barometro

String dados_string = ""; // irá conter os dados dos sensores
float tempo_atual;



//----------------------------------------------------------------------------------
void setup() {
#if EXIST_IGN_1
  pinMode(IGN_1, OUTPUT);
  digitalWrite(IGN_1, LOW);
#endif
#if EXIST_IGN_2
  pinMode(IGN_2, OUTPUT);
  digitalWrite(IGN_2, LOW);
#endif
#if EXIST_IGN_3
  pinMode(led_final, OUTPUT);
  digitalWrite(led_final, LOW);
#endif

  Serial.begin(115200);
  Wire.begin();

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

#if EXIST_GIRO
  if (!giroscopio.init()) {
    Serial.println("Failed to autodetect gyro type!");
  }
  giroscopio.enableDefault();
#endif

#if EXIST_ACEl
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  }
  accel.setRange(ADXL345_RANGE_16_G);
#endif

#if EXIST_MAG
  if (!mag.begin()) {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
  }
#endif

  // calcula a media dos dados para referenciar a altura
#if EXIST_BAR
  Call_chao();
#endif

  dados_string = "";
#if EXIST_TEMPO
  dados_string += "Tempo atual(s):\t";
#endif
#if EXIST_ALTURA
  for (int i = 0; i < NUMERO_FILTROS; i++) {
    dados_string += "Alt do filtro";
    dados_string += String(i+1);
    dados_string += " (m):\t";
  }
  dados_string += "Detecção de queda:\t";
#endif
#if EXIST_IGN_1
  dados_string += "Estado do led piloto:\t";
#endif
#if EXIST_IGN_2
  dados_string += "Estado do led secundario:\t";
#endif
#if EXIST_IGN_3
  dados_string += "Estado do led Final:\t";
#endif

#if EXIST_GIRO
#if EXIST_GIRO_X
  dados_string += "Giroscopio em X:\t";
#endif
#if EXIST_GIRO_Y
  dados_string += "Giroscopio em Y:\t";
#endif
#if EXIST_GIRO_Z
  dados_string += "Giroscopio em Z:\t";
#endif
#endif

#if EXIST_ACEl
#if EXIST_ACEl_X
  dados_string += "Acelerometro em X(m/s^2):\t";
#endif
#if EXIST_ACEl_Y
  dados_string += "Acelerometro em Y(m/s^2):\t";
#endif
#if EXIST_ACEl_Z
  dados_string += "Acelerometro em Z(m/s^2):\t";
#endif
#endif

#if EXIST_MAG
#if EXIST_MAG_X
  dados_string += "Sensormag em X(uT):\t";
#endif
#if EXIST_MAG_Y
  dados_string += "Sensormag em Y(uT):\t";
#endif
#if EXIST_MAG_Z
  dados_string += "Sensormag em Z(uT):\t";
#endif
#endif


  // laço para determinar o nome do SD
#if EXIST_SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  int(numero_do_SD) = 0;
  String(nome_SD) = "Kob_";
  String(txt_SD) = ".txt";
  do {
    String(zeros) = "";
    projeto_name = "";
    String(complemento_SD) = String(numero_do_SD);
    qnt_zero = 8 - nome_SD.length() - complemento_SD.length();
    for (int i = 0; i < qnt_zero; i++) {
      zeros += "0";
    }
    projeto_name = nome_SD + zeros + complemento_SD + txt_SD;
    numero_do_SD ++;
  } while (SD.exists(projeto_name));
  Serial.println(projeto_name);

  File dataFile = SD.open(projeto_name, FILE_WRITE);
  if (dataFile) {
#if EXIST_BAR
    dataFile.print("Constante que referencia o chão:");
    dataFile.println(ref_chao);
#endif //barometro
    dataFile.println(dados_string);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o SD");
  }
  dataFile.close();
#endif //SD

#if EXIST_BAR
 // Serial.print("Constante que referencia o chão:");
 // Serial.println(ref_chao);
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

  tempo_atual = millis();
#if EXIST_BAR
  Filtros(bmp.readAltitude() - ref_chao);
  var_queda = Detec_queda();
  time_do_apogeu = Hora_apogeu();
#endif
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

#if EXIST_BAR
void Call_chao() {
  float z = 0;
  for (int x = 0; x < MEDIA_PARA_CHAO; x++) {
    z = bmp.readAltitude() + z;
  }
  ref_chao = z / MEDIA_PARA_CHAO;
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_BAR
void Filtros(float valor) {
  sinal[0] = valor;
  for (int y = 0; y < NUMERO_FILTROS; y++) {
    sinal[y + 1] =  Calculo_media_movel(y, sinal[y]);
  }
}

#endif

//----------------------------------------------------------------------------------

#if EXIST_BAR
 float Calculo_media_movel(int camada, float valor) {
   for (int x = INTERVALO_MEDIA_M - 1; x > 0; x--) {
     vetor[camada][x] = vetor[camada][x - 1];
  }
  vetor[camada][0] = valor;
  float k = 0;
  for (int x = 0; x < INTERVALO_MEDIA_M; x++) {
    k = vetor[camada][x] + k;
  }
  return k / INTERVALO_MEDIA_M;
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_BAR
 boolean Detec_queda() {

  if (sinal[NUMERO_FILTROS] < sinalzin){
    arm_queda ++;
  }else {
    arm_queda = 0;
    }
  
  sinalzin = sinal[NUMERO_FILTROS];
  
  return arm_queda >= SOMA_FINAL_QUEDA; // Caindo = true
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_BAR
 float Hora_apogeu() {
   if(var_queda){
     if (trava_apogeu) {
     time_do_apogeu = tempo_atual;
       trava_apogeu = false; 
     }
   }
  return time_do_apogeu;
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_IGN_1
void Led_para_queda_piloto() {
  if (var_queda){ 
    if (trava_piloto) {
      digitalWrite(IGN_1, HIGH);
      acendeu_piloto = 1;
      trava_piloto = false;
    }
    if (trava_piloto == false) {
      if ((tempo_atual - time_do_apogeu) >= TEMPO_PILOTO_ON) {
        digitalWrite(IGN_1, LOW);
        acendeu_piloto = 0;
      }
    }
  }
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_IGN_2
void Led_para_queda_secundario() {
  if (var_queda) {
    if (trava_secundario) {
      if ((tempo_atual - time_do_apogeu) >= TEMPO_ATIVAR_SECUNDARIO) {
        digitalWrite(IGN_2, HIGH);
        time_secundario = tempo_atual;
        acendeu_secundario = 1;
        trava_secundario = false;
      }
    }
    if (trava_secundario == false) {
      if ((tempo_atual - time_secundario) >= TEMPO_SECUNDARIO_ON) {
        digitalWrite(IGN_2, LOW);
        acendeu_secundario = 0;
      }
    }
  }
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_IGN_3
void Led_para_queda_final() {
  if (var_queda) {
    if (sinal[NUMERO_FILTROS] <= ALTURA_DE_ATIVACAO) {
      if (trava_final) {
        digitalWrite(led_final, HIGH);
        time_final = tempo_atual;
        acendeu_final = 1;
        trava_final = false;
      }
      if (trava_final == false) {
        if ((tempo_atual - time_final) >= TEMPO_FINAL_ON) {
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
void Giroscopio() {
  giroscopio.read();
  estado_giroscopio = "";
#if EXIST_GIRO_X
  estado_giroscopio += giroscopio.g.x;
  estado_giroscopio += "\t";
#endif
#if EXIST_GIRO_Y
  estado_giroscopio += giroscopio.g.y;
  estado_giroscopio += "\t";
#endif
#if EXIST_GIRO_Z
  estado_giroscopio += giroscopio.g.z;
#endif
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_ACEl
void Acelerometro() {
  sensors_event_t event;
  accel.getEvent(&event);
  estado_acelerometro = "";
#if EXIST_ACEl_X
  estado_acelerometro += String(event.acceleration.x);
  estado_acelerometro += "\t";
#endif
#if EXIST_ACEl_Y
  estado_acelerometro += String(event.acceleration.y);
  estado_acelerometro += "\t";
 // estado_acelerometro += String(Calculo_media_movel(NUMERO_FILTROS,event.acceleration.y));
#endif
#if EXIST_ACEl_Z
  estado_acelerometro += String(event.acceleration.z);
#endif
}
#endif

//----------------------------------------------------------------------------------

#if EXIST_MAG
void Magsensor() {
  sensors_event_t event;
  mag.getEvent(&event);
  estado_sensormag = "";
#if EXIST_MAG_X
  estado_sensormag += String(event.magnetic.x);
  estado_sensormag += "\t";
#endif
#if EXIST_MAG_Y
  estado_sensormag += String(event.magnetic.y);
  estado_sensormag += "\t";
#endif
#if EXIST_MAG_Z
  estado_sensormag += String(event.magnetic.z);
#endif
}
#endif

//----------------------------------------------------------------------------------

void Salvar() {
  dados_string = "";
#if EXIST_TEMPO
  dados_string += String(tempo_atual / 1000);
#endif
#if EXIST_ALTURA
  dados_string += "\t";
  // dados_string += String(sinal[NUMERO_FILTROS]);
  // dados_string += "\t";
  for (int x = NUMERO_FILTROS ; x >= 0; x--) {
    dados_string += String(sinal[x]);
    dados_string += "\t";
  }
  dados_string += String(var_queda,DEC);
#endif
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
  File dataFile = SD.open(projeto_name, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dados_string);
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o SD");
  }
#endif
}

//----------------------------------------------------------------------------------
