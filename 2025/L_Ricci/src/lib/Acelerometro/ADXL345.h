#ifndef _ADXL345_h
#define _ADXL345_h

#include <Wire.h>
#include "../Eixos/Eixos.h"

class ADXL345 : public Eixos {
private:
  int X0, X1, X_out;
  int Y0, Y1, Y_out;
  int Z0, Z1, Z_out;
  float scale;
  uint8_t range;
public:
  ADXL345(uint8_t range = 2);
  bool begin();
  bool readAll();
};
#endif