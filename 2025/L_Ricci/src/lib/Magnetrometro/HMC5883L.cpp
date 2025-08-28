#include "HMC5883L.h"
#include <Arduino.h>

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
  estado = Wire.endTransmission();

  if (estado) {
    if (millis() - ultimoTempoResposta > 1000) {
      begin();
      ultimoTempoResposta = millis();
    }

    Wire.requestFrom((uint8_t)0x1E, (uint8_t)6);
    ultimoTempoResposta = millis();
    if (Wire.available() == 6) {
      x = Wire.read() << 8 | Wire.read();
      y = Wire.read() << 8 | Wire.read();
      z = Wire.read() << 8 | Wire.read();
    }
  }


  return true;
}