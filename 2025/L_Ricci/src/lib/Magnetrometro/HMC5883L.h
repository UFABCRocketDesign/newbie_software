#ifndef _HMC5883L_h
#define _HMC5883L_h

#include <Wire.h>
#include "../Eixos/Eixos.h"

class HMC5883L : public Eixos {
public:
  bool begin();
  bool readAll();
};
#endif