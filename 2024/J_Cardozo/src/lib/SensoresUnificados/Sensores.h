#ifndef SENSOR_H
#define SENSOR_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Sensor {
private: 
    const uint8_t address;
public:
    Sensor(uint8_t endereco);
    virtual bool begin() = 0; 
    virtual void lerTudo() = 0; 
};

#endif
