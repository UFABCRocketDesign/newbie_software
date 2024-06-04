#include "ADXL345.h"

ADXL345::ADXL345(uint8_t range) : range(range) {
  switch (range) {
    case 2:
      scale = 256.0;
      break;
    case 4:
      scale = 128.0;
      break;
    case 8:
      scale = 64.0;
      break;
    default:
      scale = 256.0;
  }
}

bool ADXL345::begin() {
  Wire.beginTransmission(ADXAddress);
  Wire.write(Reg_2D);
  Wire.write(8);
  return (Wire.endTransmission() == 0);
}

void ADXL345::lerTudo() {
  Wire.beginTransmission(ADXAddress);
  Wire.write(Reg_X0); 
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress, 6); 

  if (Wire.available() == 6) { 
    X0 = Wire.read();
    X1 = Wire.read();
    Y0 = Wire.read();
    Y1 = Wire.read();
    Z0 = Wire.read();
    Z1 = Wire.read();

    X1 = X1 << 8;
    X_out = X0 + X1;

    Y1 = Y1 << 8;
    Y_out = Y0 + Y1;

    Z1 = Z1 << 8;
    Z_out = Z0 + Z1;
  }

  Xg = X_out / scale;
  Yg = Y_out / scale;
  Zg = Z_out / scale;
}

float ADXL345::getX() {
  return Xg;
}

float ADXL345::getY() {
  return Yg;
}

float ADXL345::getZ() {
  return Zg;
}