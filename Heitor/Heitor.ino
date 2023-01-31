#include <Adafruit_BMP085.h>
// #include "../pinos.h"

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif // ARDUINO_AVR_MEGA2560

#define IGN_1 36	/*act1*/
#define IGN_2 61	/*act2*/
#define IGN_3 46	/*act3*/
#define IGN_4 55	/*act4*/

#define use_buzzer 0
#define print_aux 1

#define apg_limiar 30

#define PCHT_INTERVAL 5000
#define PCHT_DELAY 2000
#define PCHY_HEIGHT -2

#define len 15
#define lvl 3
#define coiso vec[lvl][len]
#define idx(I) vec[(I)][index[(I)]]
#define av_pp(I) av[(I)] += idx(I)
#define av_ll(I) av[(I)] -= idx(I)
#define av_tt(I) av[(I)] *= len
#define av_dd(I) av[(I)] /= len
// #define update(X) ((++X)%len)
#define update(X) (((++X)<(len))?((X)):0)
#define av_fst(I) av_tt(I);av_ll(I)
#define av_lst(I) av_pp(I);av_dd(I)

float coiso = {};
int index[lvl] = {};
float av[lvl] = {};
float last_val = 0.0;

int apg_counter = 0;
bool apg = false;
float apg_time;

Adafruit_BMP085 bmp;
float solo = 0.0;

bool pchtA = false;
char pchtA_seal = 0;
long pchtA_time;

bool pchtB = false;
char pchtB_seal = 0;
long pchtB_time;

bool pchtC = false;
char pchtC_seal = 0;
long pchtC_time;

void setup() {
  pinMode(IGN_1,OUTPUT);
  pinMode(IGN_2,OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  pinMode(LED_BUILTIN,OUTPUT);
#if use_buzzer
  pinMode(A0,OUTPUT);
  digitalWrite(A0,LOW);
  delay(250);
  digitalWrite(A0,HIGH);
#endif
//   Serial.print("*C\tPa\tm\t" );
  Serial.print("raw");
  for(int i=0; i<lvl; i++)
  {
    Serial.print("\tav");
    Serial.print(i);
  }
#if print_aux
  Serial.print("\testado");
  Serial.print("\tdetector");
#endif
  Serial.println();

  for(int i=0; i<100; i++) solo += bmp.readAltitude();
  solo /= 100.0;
}

void loop() {

  long now = millis();

  String datalog = "";
  float temp = bmp.readTemperature();
  int press = bmp.readPressure();
  float alt = bmp.readAltitude()-solo;

  for(int i=0; i<lvl; i++)
  {
    av_fst(i);
    if(i==0) idx(i) = alt;
    else idx(i) = av[i-1];
    av_lst(i);
    index[i] = update(index[i]);
  }

//   datalog+=String(temp);
//   datalog+="\t";
//   datalog+=String(press);
//   datalog+="\t";
  datalog+=String(alt);

  for(int i=0; i<lvl; i++)
  {
    datalog+="\t";
    datalog+=String(av[i]);
  }

#if print_aux
  datalog+="\t";
#endif

  float curr_val = av[lvl-1];
  if(last_val > curr_val)
  {
#if print_aux
    datalog+="1";
#endif
    // digitalWrite(LED_BUILTIN,HIGH);
    apg_counter++;
  }
  else
  {
#if print_aux
    datalog+="-1";
#endif
    // digitalWrite(LED_BUILTIN,LOW);
    apg_counter=0;
  }
  last_val = curr_val;

#if print_aux
  datalog+="\t";
  datalog+=String(float(min(apg_counter,apg_limiar))/apg_limiar);
#endif

  if(apg_counter>=apg_limiar && !apg){
    apg = true;
    apg_time = now;
  }

  if(apg){
    if(pchtA_seal == 0){
      pchtA_seal++;
      pchtA_time = now + PCHT_INTERVAL;
      pchtA = HIGH;
    }
    if(pchtA_seal == 1 && now > pchtA_time){
      pchtA_seal++;
      pchtA = LOW;
    }

    if(pchtB_seal == 0){
      pchtB_seal++;
      pchtB_time = now + PCHT_DELAY;
    }
    if(pchtB_seal == 1 && now > pchtB_time){
      pchtB_seal++;
      pchtB_time = now + PCHT_INTERVAL;
      pchtB = HIGH;
    }
    if(pchtB_seal == 2 && now > pchtB_time){
      pchtB_seal++;
      pchtB = LOW;
    }

    if(pchtC_seal == 0 && curr_val < PCHY_HEIGHT){
      pchtC_seal++;
      pchtC_time = now + PCHT_INTERVAL;
      pchtC = HIGH;
    }
    if(pchtC_seal == 1 && now > pchtC_time){
      pchtC_seal++;
      pchtC = LOW;
    }

    digitalWrite(IGN_1, pchtA);
    digitalWrite(IGN_2, pchtB);
    digitalWrite(LED_BUILTIN, pchtC);
  }
  datalog += '\t' + String(pchtA);
  datalog += '\t' + String(pchtB);
  datalog += '\t' + String(pchtC);

#if use_buzzer
  if(apg_counter>=apg_limiar) digitalWrite(A0,LOW);
  else digitalWrite(A0,HIGH);
#endif

  Serial.println(datalog);
}
