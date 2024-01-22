#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float local;
int i;
float med;
float sum;
float base;

void setup() {

  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  i = 0;
  while(i<50) {
  local = bmp.readAltitude();
  sum = sum + local;
  i = i + 1;
} 
  base = sum/50;

  Serial.print("Temperature:");
  Serial.print("\t");
  Serial.print("Pressure:");
  Serial.print("\t");
  Serial.println("Altitude:");
}
  
void loop() {

  for (int j = 0; j<10; j++) {
  med = med + bmp.readAltitude();
  }
                 
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.println(med-base);

}