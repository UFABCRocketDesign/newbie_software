#ifndef _HMC5883L_h
#define _HMC5883L_h

#include <Wire.h>

class HMC5883L {
private:
  int x, y, z;
public:
  bool begin();
  bool readAll();
  int getX();
  int getY();
  int getZ();
};
#endif