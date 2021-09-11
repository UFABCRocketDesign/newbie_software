#include <Adafruit_BMP085.h>

#define len 10
#define coiso vec[len]
#define idx vec[index]
#define av_pp av += idx
#define av_ll av -= idx
#define av_tt av *= len
#define av_dd av /= len
#define update(X) ((++X)%len)
#define av_fst av_tt;av_ll
#define av_lst av_pp;av_dd

float coiso;
int index = 0;
float av = 0;

Adafruit_BMP085 bmp;
float solo = 0.0;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
//   Serial.print("*C\tPa\tm\t" );
  Serial.print("m");
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

  av_fst;
  idx = alt;
  av_lst;
  index = update(index);

//   Serial.print(temp);
//   Serial.print("\t");
//   Serial.print(press);
//   Serial.print("\t");
  Serial.print(alt);
  Serial.print("\t");
  Serial.print(av);
  Serial.print("\t");

  Serial.println();
}
