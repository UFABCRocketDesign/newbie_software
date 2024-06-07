#ifndef ACEL_H
#define ACEL_H

#include "../SensoresUnificados/Sensores.h"
#include "../Eixos/XYZ.h"
#include <Wire.h>

#define Reg_2D 0x2D
#define Reg_X0 0x32
#define ADXAddress 0x53 


class ADXL345 : public Sensor, public XYZ {
private:
  int X0,X1,X_out;
  int Y0,Y1,Y_out;
  int Z1,Z0,Z_out;
  float x, y, z;
  uint8_t range;
  float scale;

public:
  ADXL345(uint8_t range = 2);
  bool begin();
  void lerTudo();
};

#endif
