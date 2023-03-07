#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float alt_inicial;
float alt;
int i;

 #define N 5


  float V[N] = {};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  Serial.println(" Temperature(*C) | Pressure(Pa) | Altitude(meters)");
  Serial.println();
   
  for(i=0; i<5; i++){
  alt += bmp.readAltitude();
  }
  alt_inicial = alt/5;
}
void loop() {

float L = bmp.readAltitude() - alt_inicial;

    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(L);
    Serial.print("\t");

    for(int i = N-1; i>0; i--){
    V[i] = V[i-1];
    }
    V[0] = L;
    float sum = 0.0;
    for (int i = 0; i<N; i++){
      sum += V[i];

  }
  float media = sum / N;

  Serial.print(media);
  Serial.println();
  delay(5);
}