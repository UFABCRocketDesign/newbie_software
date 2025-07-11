#ifndef _BMP085_h
#define _BMP085_h

class BMP085 {
private:
  int ac1, ac2, ac3;
  unsigned int ac4, ac5, ac6;
  long b1, b2, b3, b5, b6;
  int mb, mc, md;
  unsigned int ut;
  unsigned long up;
  long x1, x2, x3;
  unsigned long b4, b7;
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