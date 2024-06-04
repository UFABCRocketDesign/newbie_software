#ifndef BMP085_H
#define BMP085_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#define BMP085_ADDRESS 0x77

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
    float altitude;

    long lerCalibracaoT();
    long lerCalibracaoP();
    
public:
    bool begin();
    void lerTudo(float pressaoInicial = 101325.0);
    float getTemperatura();
    long getPressao();
    float getAltitude();
};

#endif
