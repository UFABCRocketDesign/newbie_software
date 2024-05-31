#ifndef BMP085_H
#define BMP085_H

#include <Wire.h>

class BMP085 {
private:
    int ac1, ac2, ac3;
    unsigned int ac4, ac5, ac6;
    int mb, mc, md;
    long b1, b2, b3, b5, b6;
    unsigned long b4, b7;
    long x1, x2, x3;
    unsigned int ut = 0;
    unsigned long up = 0;
    const int oss = 0; // over sampling setting
    float temperatura;
    long pressao;
    float altura;

    long lerCalibracaoT();
    long lerCalibracaoP();
    
public:
    BMP085();
    void begin();
    void lerTudo();
    float getTemperatura();
    long getPressao();
    float getAltitude();
};

#endif
