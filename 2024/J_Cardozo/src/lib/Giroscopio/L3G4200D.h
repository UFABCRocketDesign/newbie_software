#ifndef L3G4200D_H
#define L3G4200D_H

#include <Wire.h>

#define CTRL_REG1 0x20
#define CTRL_REG4 0x23
#define OUT_X_L 0x28
#define L3G4200D_Address 0x69

class L3G4200D {
private:
    int16_t x, y, z;
    uint8_t odr; //Taxa de Dados de Saída
    uint8_t scale; //Faixa de medicao
    uint8_t getODR(uint16_t odr);
    uint8_t getScale(uint16_t scale);

public:
    L3G4200D(uint16_t odr = 100, uint16_t scale = 250);
    bool begin();
    void lerTudo();
    float getX();
    float getY();
    float getZ();
};

#endif