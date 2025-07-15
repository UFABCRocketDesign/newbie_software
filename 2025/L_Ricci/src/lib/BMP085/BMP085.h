#ifndef _BMP085_h
#define _BMP085_h

class BMP085 {
private:
  int ac1, ac2, ac3;
  unsigned int ac4, ac5, ac6;
  long b1, b2, b3, b5, b6;
  int mb, mc, md;
  long x1, x2, x3;
  unsigned long b4, b7;
  unsigned int ut = 0;
  unsigned long up = 0;
  const int oss = 0;
  float celsius;
  long pressao;
  float altitude;
public:
  bool begin();
  bool readAll(float pressaoInicial);
  float readTemperature();
  float readPressure();
  float readAltitude();
};
#endif