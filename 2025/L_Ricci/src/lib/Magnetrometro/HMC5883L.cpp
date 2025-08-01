#include "HMC5883L.h"

bool HMC5883L::begin() {
  Wire.beginTransmission(0x1E);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();

  return true;
}

bool HMC5883L::readAll() {
  Wire.beginTransmission(0x1E);
  Wire.write(0x03);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)0x1E, (uint8_t)6);
  if (Wire.available() == 6) {
    x = Wire.read() << 8 | Wire.read();
    y = Wire.read() << 8 | Wire.read();
    z = Wire.read() << 8 | Wire.read();
  }

  return true;
}

float HMC5883L::getX() {
  return x;
}

float HMC5883L::getY() {
  return y;
}

float HMC5883L::getZ() {
  return z;
}