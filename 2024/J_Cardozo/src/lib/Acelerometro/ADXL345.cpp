#include "ADXL345.h"

ADXL345::ADXL345(uint8_t range, long recalibrationValue) : range(range), Sensor(ADXAddress, recalibrationValue)
{
  switch (range)
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

bool ADXL345::begin()
{
  Wire.beginTransmission(address);
  Wire.write(Reg_2D);
  Wire.write(8);
  return (Wire.endTransmission() == 0);
}

bool ADXL345::lerTudo()
{
  bool verificador = true;
  thisRead = millis();

  Wire.beginTransmission(address);
  Wire.write(Reg_X0);
  verificador = verificador && (Wire.endTransmission() == 0);

  Wire.requestFrom((uint8_t)address, (uint8_t)6);
  unsigned long tempo = micros();
  while (Wire.available() < 6)
  {
    if (tempo + 10 < micros())
    {
      verificador = false;
      break;
    }
  }

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

  x = X_out / scale;
  y = Y_out / scale;
  z = Z_out / scale;
  }

  if (verificador)
  {
    lastRead = thisRead;
  }
  recalibrar("Acel");

  return verificador;
}
