#ifndef ACEL_H
#define ACEL_H

#include <Wire.h>

#define Reg_ID 0
#define Reg_2D 0x2D
#define Reg_X0 0x32
#define Reg_X1 0x33
#define Reg_Y0 0x34
#define Reg_Y1 0x35
#define Reg_Z0 0x36
#define Reg_Z1 0x37

class Acel {
private:
  int ADXAddress = 0x53;  
  int reading = 0;
  int val=0;
  int X0,X1,X_out;
  int Y0,Y1,Y_out;
  int Z1,Z0,Z_out;
  double Xg,Yg,Zg;

public:
  bool begin();
  void lerTudo();
  float getX();
  float getY();
  float getZ();
};

#endif
