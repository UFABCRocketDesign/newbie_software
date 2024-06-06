#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
private: 
    const uint8_t address;
public:
    Sensor(uint8_t endereco);
    virtual bool begin() = 0; 
    virtual void lerTudo() = 0; 
};

#endif
