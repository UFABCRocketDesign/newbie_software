#include "ADXL345.h"
#include <Arduino.h>

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
    scale = 256.0;
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
  estado = Wire.endTransmission();

  if (estado) {
    if (millis() - ultimoTempoResposta > 1000) {
      begin();
      ultimoTempoResposta = millis();
    }

    Wire.requestFrom((uint8_t)0x53, (uint8_t)6);
    ultimoTempoResposta = millis();
    if (Wire.available() == 6) {
      X0 = Wire.read();
      X1 = Wire.read();
      Y0 = Wire.read();
      Y1 = Wire.read();
      Z0 = Wire.read();
      Z1 = Wire.read();
    }

    X1 = X1 << 8;
    X_out = X0 + X1;

    Y1 = Y1 << 8;
    Y_out = Y0 + Y1;

    Z1 = Z1 << 8;
    Z_out = Z0 + Z1;

    x = (X_out / scale) * 9.80665;
    y = (Y_out / scale) * 9.80665;
    z = (Z_out / scale) * 9.80665;
  }
  return true;
}