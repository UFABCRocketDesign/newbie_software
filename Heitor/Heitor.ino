#include <Adafruit_BMP085.h>
// #include "../pinos.h"

#define use_buzzer 1
#define print_aux 0

#define apg_limiar 60

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

Adafruit_BMP085 bmp;
float solo = 0.0;

void setup() {
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
    datalog+=String(1);
#endif
    digitalWrite(LED_BUILTIN,HIGH);
    apg_counter++;
  }
  else
  {
#if print_aux
    datalog+=String(-1);
#endif
    digitalWrite(LED_BUILTIN,LOW);
    apg_counter=0;
  }
  last_val = curr_val;

#if print_aux
  datalog+="\t";
  datalog+=String(float(min(apg_counter,apg_limiar))/apg_limiar);
#endif

#if use_buzzer
  if(apg_counter>=apg_limiar) digitalWrite(A0,LOW);
  else digitalWrite(A0,HIGH);
#endif

  Serial.println(datalog);
}
