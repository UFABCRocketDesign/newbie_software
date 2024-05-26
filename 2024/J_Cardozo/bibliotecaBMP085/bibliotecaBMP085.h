#ifndef BMP085_H
#define BMP085_H

#include <Wire.h>

class BMP085 {
private:
    short ac1, ac2, ac3;
    unsigned short ac4, ac5, ac6;
    short mb, mc, md;
    long b1, b2, b3, b5, b6;
    unsigned long b4, b7;
    long x1, x2, x3;
    unsigned long ut = 0;
	unsigned long up = 0;
    const int oss = 0; //overSamplingsSetting
    float t;
    long p;

    float lerCalibracaoT();
    float lerCalibracaoP();
    int16_t read(uint8_t reg, uint8_t bytes);
    void write8(uint8_t reg, uint8_t value);

public:
    BMP085();
    float begin();
    float lerTempemperatura();
    int32_t lerPressao();
    float lerAltitude(float pressaoAlturaMar = 101325);
    
}