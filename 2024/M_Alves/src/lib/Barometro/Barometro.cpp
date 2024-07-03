#include "Barometro.h"

Barometro::Barometro() {
}

bool Barometro::begin() {
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xAA);
  if (Wire.endTransmission() != 0) {
    return false; // A transmissão falhou
  }

  Wire.requestFrom(BMP085_ADDRESS, 22);

  unsigned long intervalo = micros();
  while (Wire.available() < 22) {
    if (intervalo + 10 < micros()) {
      return false; 
      }
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

  return true; // A calibração foi boa
}

bool Barometro::getAll() {
  // Leitura de temperatura
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  delayMicroseconds(4550);
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 2);
  unsigned long intervalo = micros();
  while (Wire.available() < 2) {
    if (intervalo + 10 < micros())break;
    }
  unsigned int ut = Wire.read() << 8 | Wire.read();

  // Leitura de pressao
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS << 6));
  Wire.endTransmission();
  delay(2 + (3 << OSS));
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  intervalo = micros();
  while (Wire.available() < 3) {
    if (intervalo + 10 < micros())break;
    }
  unsigned long up = (((unsigned long) Wire.read() << 16) | ((unsigned long) Wire.read() << 8) | (unsigned long) Wire.read()) >> (8-OSS);

  // Calculo de temperatura
  long x1, x2;
  x1 = ((long)ut - (long)ac6)*(long)ac5 >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;
  temperatura = ((b5 + 8)>>4) / 10.0;

  // Calculo de pressao
  long b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6>>12))>>11;
  x2 = (ac2 * b6)>>11;
  long x3 = x1 + x2;
  long b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * (b6 * b6>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  unsigned long b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  unsigned long b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    pressao = (b7<<1)/b4;
  else
    pressao = (b7/b4)<<1;
  x1 = (pressao>>8) * (pressao>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * pressao)>>16;
  pressao += (x1 + x2 + 3791)>>4;

  // Calculo de altitude
  float A = pressao/101325;
  float B = 1/5.25588;
  float C = pow(A,B);
  C = 1 - C;
  altitude = C /0.0000225577;

  return true; // A leitura foi bem-sucedida
}

float Barometro::getTemperatura() {
  return temperatura;
}

long Barometro::getPressao() {
  return pressao;
}

float Barometro::getAltitude() {
  return altitude;
}