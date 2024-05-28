#ifndef BMP085_H
#define BMP085_H

#include <Wire.h>

class BMP085 {
private:
    int16_t ac1, ac2, ac3;
    uint16_t ac4, ac5, ac6;
    int16_t mb, mc, md;
    int32_t b1, b2, b3, b5, b6;
    uint32_t b4, b7;
    int32_t x1, x2, x3;
    uint32_t ut = 0;
    uint32_t up = 0;
    const int oss = 0; // over sampling setting
    float t;
    int32_t p;
    float h;

    float lerCalibracaoT();
    float lerCalibracaoP();
    int16_t read(uint8_t reg, uint8_t bytes);
    void write8(uint8_t reg, uint8_t value);

public:
    BMP085();
    void begin();
    void lerTudo();
    float getTempemperatura();
    int32_t getPressao();
    float getAltitude();
};

#endif
