#include "BMP085.h"
#include <Wire.h>
#include <Arduino.h>

bool BMP085::begin() {
  Wire.beginTransmission(0x77);
  Wire.write(0XAA);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)0x77, (uint8_t)22);
  unsigned long tempo = micros();
  while (Wire.available() < 22) {
    if (tempo + 10 < micros()) break;
  }

  ac1 = Wire.read() << 8 | Wire.read();
  ac2 = Wire.read() << 8 | Wire.read();
  ac3 = Wire.read() << 8 | Wire.read();
  ac4 = Wire.read() << 8 | Wire.read();
  ac5 = Wire.read() << 8 | Wire.read();
  ac6 = Wire.read() << 8 | Wire.read();
  b1 = Wire.read() << 8 | Wire.read();
  b2 = Wire.read() << 8 | Wire.read();
  mb = Wire.read() << 8 | Wire.read();
  mc = Wire.read() << 8 | Wire.read();
  md = Wire.read() << 8 | Wire.read();

  return true;
}

bool BMP085::readAll(float pressaoInicial) {

  Wire.beginTransmission(0x77);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  delayMicroseconds(4550);

  Wire.beginTransmission(0x77);
  Wire.write(0xF6);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)0x77, (uint8_t)2);
  unsigned long tempo = micros();
  while (Wire.available() < 2) {
    if (tempo + 10 < micros()) break;
  }
  ut = Wire.read() << 8 | Wire.read();

  Wire.beginTransmission(0x77);
  Wire.write(0xF4);
  Wire.write(0x34 + (oss << 6));
  Wire.endTransmission();

  delay(2 + (3 << oss));

  Wire.beginTransmission(0x77);
  Wire.write(0xF6);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)0x77, (uint8_t)3);
  tempo = micros();
  while (Wire.available() < 3) {
    if (tempo + 10 < micros()) break;
  }

  up = (((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | (unsigned long)Wire.read()) >> (8 - oss);

  Wire.beginTransmission(0x77);

  // True Temperature
  x1 = (ut - (long)ac6) * (long)ac5 >> 15;
  x2 = ((long)mc << 11) / (x1 + md);
  b5 = x1 + x2;
  celsius = (float)((b5 + 8) >> 4)/ 10.0;

  // True Pressure
  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 >> 12)) >> 11;
  x2 = (ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = ((((long)ac1 * 4 + x3) << oss) + 2) >> 2;

  x1 = (ac3 * b6) >> 13;
  x2 = (b1 * (b6 * b6 >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

  b7 = ((unsigned long)(up - b3) * (50000 >> oss));
  if (b7 < 0x80000000) {
    pressao = (b7 << 1) / b4;
  } else {
    pressao = (b7 / b4) << 1;
  }

  x1 = (pressao >> 8) * (pressao >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * pressao) >> 16;
  pressao = pressao + ((x1 + x2 + 3791) >> 4);

  altitude = 44330.0 * (1.0 - pow((pressao / pressaoInicial), (1.0/5.255)));

  return true;
}

float BMP085::readTemperature() {
  return celsius;
}

long BMP085::readPressure() {
  return pressao;
}

float BMP085::readAltitude() {
  return altitude;
}