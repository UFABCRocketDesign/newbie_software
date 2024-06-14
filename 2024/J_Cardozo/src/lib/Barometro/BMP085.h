#ifndef BMP085_H
#define BMP085_H

#include "../SensoresUnificados/Sensores.h"
#include <Wire.h>

#define BMP085_ADDRESS 0x77

class BMP085 : public Sensor {
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
    BMP085();
    bool begin();
    bool lerTudo(float pressaoInicial);
    bool lerTudo();
    float getTemperatura();
    long getPressao();
    float getAltitude();
};

#endif
