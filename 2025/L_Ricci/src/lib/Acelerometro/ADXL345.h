#ifndef _ADXL345_h
#define _ADXL345_h

#include <Wire.h>

class ADXL345 {
private:
  int X0, X1, X_out;
  int Y0, Y1, Y_out;
  int Z0, Z1, Z_out;
  float x, y, z;
  float scale;
  uint8_t range;
public:
  ADXL345(uint8_t range = 2);
  bool begin();
  bool readAll();
  float getX();
  float getY();
  float getZ();
};
#endif