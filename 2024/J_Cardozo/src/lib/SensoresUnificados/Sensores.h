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
    int recalTime;
    int timerTime;
    bool estaDescendoTimer = false;
    long getTime();

public:
    Sensor(uint8_t endereco, int recalibracao = 7000, int timer = 14000);
    virtual bool begin() = 0; 
    virtual bool lerTudo() = 0; 
    void recalibrar(String sensor);
    void timer();
    bool getEstaDescendoTimer();
};

#endif
