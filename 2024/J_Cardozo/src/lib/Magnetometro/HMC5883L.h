#ifndef HMC5883L_H
#define HMC5883L_H

#include <Wire.h>

#define HMC5883L_Address 0x1E

class HMC5883L {
private:
    int16_t x, y, z;
    uint8_t configA;
    uint8_t configB;
    uint8_t mode;
    uint8_t getGain(uint16_t gain);
    uint8_t getSamples(uint8_t samples);
    uint8_t getMode(uint8_t mode);

public:
    HMC5883L(uint8_t samples = 8, uint16_t gain = 1090, uint8_t mode = 0);
    bool begin();
    void lerTudo();
    int getX();
    int getY();
    int getZ();
};

#endif
