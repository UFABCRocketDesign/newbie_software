#ifndef _SENSORES_H
#define _SENSORES_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Sensor {
protected: 
    const uint8_t address;
public:
    Sensor(uint8_t endereco);
    virtual bool begin() = 0; 
    virtual bool getAll() = 0; 
};

#endif