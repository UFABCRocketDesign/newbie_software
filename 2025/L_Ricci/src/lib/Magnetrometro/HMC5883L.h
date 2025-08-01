#ifndef _HMC5883L_h
#define _HMC5883L_h

#include <Wire.h>

class HMC5883L {
private:
  float x, y, z;
public:
  bool begin();
  bool readAll();
  float getX();
  float getY();
  float getZ();
};
#endif