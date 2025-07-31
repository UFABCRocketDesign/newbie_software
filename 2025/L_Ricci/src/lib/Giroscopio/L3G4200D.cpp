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

  Wire.requestFrom(0x69, (uint8_t)6);
  if (Wire.available() == 6) {
    uint8_t X0 = Wire.read();
    uint8_t X1 = Wire.read();
    uint8_t Y0 = Wire.read();
    uint8_t Y1 = Wire.read();
    uint8_t Z0 = Wire.read();
    uint8_t Z1 = Wire.read();
  }

  x = (int16_t)(X0 | (X1 << 8));
  Serial.println(x);
  y = (int16_t)(Y0 | (Y1 << 8));
  Serial.println(y);
  z = (int16_t)(Z0 | (Z1 << 8));
  Serial.println(z);

  return true;
}

float L3G4200D::getX() {
  return x;
}

float L3G4200D::getY() {
  return y;
}

float L3G4200D::getZ() {
  return z;
}