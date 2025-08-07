#include "L3G4200D.h"
#include <Arduino.h>

bool L3G4200D::begin() {
  Wire.beginTransmission(0x69);
  Wire.write(0x20);
  Wire.write(0b00001111);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x69);
  Wire.write(0x21);
  Wire.write(0b00000000);
  Wire.endTransmission();

  Wire.beginTransmission(0x69);
  Wire.write(0x22);
  Wire.write(0b00001000);
  Wire.endTransmission();

  Wire.beginTransmission(0x69);
  Wire.write(0x23);
  switch (scale) {
    case 250:
      Wire.write(0b00000000);
      break;
    case 500:
      Wire.write(0b00010000);
      break;
    default:
      Wire.write(0b00110000);
      break;
  }
  Wire.endTransmission();

  Wire.beginTransmission(0x69);
  Wire.write(0x24);
  Wire.write(0b00000000);
  Wire.endTransmission();

  return true;
}

bool L3G4200D::readAll() {
  Wire.beginTransmission(0x69);
  Wire.write(0x28 | 0x80); // O que essa parte faz?
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)0x69, (uint8_t)6);
  if (Wire.available() == 6) {
    X0 = Wire.read();
    X1 = Wire.read();
    Y0 = Wire.read();
    Y1 = Wire.read();
    Z0 = Wire.read();
    Z1 = Wire.read();
  }

  x = (int16_t)(X0 | (X1 << 8));
  y = (int16_t)(Y0 | (Y1 << 8));
  z = (int16_t)(Z0 | (Z1 << 8));

  return true;
}