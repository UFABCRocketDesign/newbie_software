#include "Acel.h"

bool Acel::begin() {
  Wire.beginTransmission(ADXAddress);
  Wire.write(Reg_2D);
  Wire.write(8);
  return (Wire.endTransmission() == 0);
}

void Acel::lerTudo() {
  //X
  Wire.beginTransmission(ADXAddress);
  Wire.write(Reg_X0);
  Wire.write(Reg_X1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    X0 = Wire.read();
    X1 = Wire.read(); 
    X1=X1<<8;
    X_out=X0+X1;   
  }

  //Y
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_Y0);
  Wire.write(Register_Y1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Y0 = Wire.read();
    Y1 = Wire.read(); 
    Y1=Y1<<8;
    Y_out=Y0+Y1;
  }

  //Z
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_Z0);
  Wire.write(Register_Z1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Z0 = Wire.read();
    Z1 = Wire.read(); 
    Z1=Z1<<8;
    Z_out=Z0+Z1;
  }

  Xg=X_out/256.0;
  Yg=Y_out/256.0;
  Zg=Z_out/256.0;
}

float Acel::getX() {
  return Xg;
}

float Acel::getY() {
  return Yg;
}

float Acel::getZ() {
  return Zg;
}
