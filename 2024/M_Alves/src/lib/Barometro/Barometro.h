#ifndef _BAROMETRO_H
#define _BAROMETRO_H

#include <Wire.h>
#include "../Sensores/Sensores.h"

#define BMP085_ADDRESS 0x77

class Barometro {
  public:
    Barometro();
    bool begin();
    bool getAll();
    float getTemperatura();
    long getPressao();
    float getAltitude();

  private:
    const unsigned char OSS = 0;  
    int ac1, ac2, ac3, mb, mc, md;
    unsigned int ac4, ac5, ac6;
    long b1, b2, b5; 
    float temperatura;
    long pressao;
    float altitude;
};

#endif // BAROMETRO_H

