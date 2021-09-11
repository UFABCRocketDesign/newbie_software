#include <Adafruit_BMP085.h>

#define len 10
#define lvl 3
#define coiso vec[lvl][len]
#define idx(I) vec[(I)][index[(I)]]
#define av_pp(I) av[(I)] += idx(I)
#define av_ll(I) av[(I)] -= idx(I)
#define av_tt(I) av[(I)] *= len
#define av_dd(I) av[(I)] /= len
#define update(X) ((++X)%len)
#define av_fst(I) av_tt(I);av_ll(I)
#define av_lst(I) av_pp(I);av_dd(I)

float coiso = {};
int index[lvl] = {};
float av[lvl] = {};

Adafruit_BMP085 bmp;
float solo = 0.0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
//   Serial.print("*C\tPa\tm\t" );
  Serial.print("raw");
  for(int i=0; i<lvl; i++)
  {
    Serial.print("\tav");
    Serial.print(i);
  }
  Serial.println();

  for(int i=0; i<100; i++)
  {
    solo += bmp.readAltitude();
  }
  solo /= 100.0;
}

void loop() {

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

//   Serial.print(temp);
//   Serial.print("\t");
//   Serial.print(press);
//   Serial.print("\t");
  Serial.print(alt);
  for(int i=lvl-1; i<lvl; i++)
  {
    Serial.print("\t");
    Serial.print(av[i]);
  }
  // Serial.print("\t");

  Serial.println();
}
