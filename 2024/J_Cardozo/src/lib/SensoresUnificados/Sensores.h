#ifndef SENSOR_H
#define SENSOR_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Sensor {
protected: 
    const uint8_t address;
    unsigned long lastRead = 0;
    unsigned long thisRead = 0;
    unsigned long recalTime;

public:
    Sensor(uint8_t endereco, long recalibracao = 7000);
    virtual bool begin() = 0; 
    virtual bool lerTudo() = 0; 
    long getTime();
    void recalibrar();
};

#endif
