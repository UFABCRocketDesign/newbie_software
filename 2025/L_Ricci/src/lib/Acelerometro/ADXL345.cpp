#include "ADXL345.h"

ADXL345::ADXL345(uint8_t range) {
  switch(range) 
  {
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
    scale = 128.0;
  }
}

bool ADXL345::begin() {
  Wire.beginTransmission(0x53);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
  return true;
}

bool ADXL345::readAll() {
  Wire.beginTransmission(0x53);
  Wire.write(0x32);
  Wire.endTransmission();

  Wire.requestFrom(0x53, 6);
  if (Wire.available() == 6) {
    X0 = Wire.read() | Wire.read() << 8;
    X1 = Wire.read();
    Y0 = Wire.read() | Wire.read() << 8;
    Y1 = Wire.read();
    Z0 = Wire.read() | Wire.read() << 8;
    Z1 = Wire.read();
  }

  X_out = X0 + X1;
  Y_out = Y0 + Y1;
  Z_out = Z0 + Z1;

  x = X_out / scale;
  y = Y_out / scale;
  z = Z_out / scale;

  return true;
}

float ADXL345::getX() {
  return x;
}

float ADXL345::getY() {
  return y;
}

float ADXL345::getZ() {
  return z;
}