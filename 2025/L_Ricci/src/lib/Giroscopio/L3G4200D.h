#ifndef _L3G4200_h
#define _L3G4200_h

#include <Wire.h>
#include "../Eixos/Eixos.h"

class L3G4200D : public Eixos {
private:
  uint8_t X0, X1;
  uint8_t Y0, Y1;
  uint8_t Z0, Z1;
  int scale;
public:
  bool begin();
  bool readAll();
};
#endif