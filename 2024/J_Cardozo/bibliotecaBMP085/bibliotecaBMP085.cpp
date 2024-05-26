#include "BMP085.h"

#define BMP085_ADDRESS 0x77

#define BMP085_AC1 0xAA
#define BMP085_AC2 0xAC
#define BMP085_AC3 0xAE
#define BMP085_AC4 0xB0
#define BMP085_AC5 0xB2
#define BMP085_AC6 0xB4
#define BMP085_B1 0xB6
#define BMP085_B2 0xB8
#define BMP085_MB 0xBA
#define BMP085_MC 0xBC
#define BMP085_MD 0xBE

uint32_t read(uint8_t reg, uint8_t bytes) {
  if (bytes < 2 || bytes > 3) {
    return 0; 
  }

  Wire.beginTransmission(BMP085_ADDRESS); 
  Wire.write(reg);                       
  Wire.endTransmission();                 
  
  Wire.requestFrom(BMP085_ADDRESS, bytes); 
  
  if (Wire.available() >= bytes) {
    uint32_t value = 0;
    for (int i = 0; i < bytes; i++) {
      value = (value << 8) | Wire.read(); 
    }
    return value; 
  } else {
    return 0;
  }
}

void write8(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(BMP085_ADDRESS); 
  Wire.write(reg);                        
  Wire.write(value);                      
  Wire.endTransmission();                 
}

void BMP085::begin() {
    int16_t AC1 = read(BMP085_AC1, 2);
    int16_t AC2 = read(BMP085_AC2, 2);
    int16_t AC3 = read(BMP085_AC3, 2);
    uint16_t AC4 = read(BMP085_AC4, 2);
    uint16_t AC5 = read(BMP085_AC5, 2);
    uint16_t AC6 = read(BMP085_AC6, 2);
    int16_t B1 = read(BMP085_B1, 2);
    int16_t B2 = read(BMP085_B2, 2);
    int16_t MB = read(BMP085_MB, 2);
    int16_t MC = read(BMP085_MC, 2);
    int16_t MD = read(BMP085_MD, 2);
}

float lerCalibracaoT() {
    write8(0xF4, 0x2E);
    delayMicroseconds(4550);

    float value = read(0xF6);
    return value;
}

float lerCalibracaoP() {
    write8(0xF4, (0x34+(OSS<<6)));
    read(0xF6);
    delay(2 + (3 << OSS));
}

