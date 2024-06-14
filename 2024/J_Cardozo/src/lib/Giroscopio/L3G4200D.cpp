#include "L3G4200D.h"

L3G4200D::L3G4200D(uint16_t odr, uint16_t scale) : odr(getODR(odr)), scale(getScale(scale)), Sensor(L3G4200D_Address) {}

uint8_t L3G4200D::getODR(uint16_t odr) {
    switch (odr) {
        case 12:
            return 0x00; //12 Hz
        case 25:
            return 0x10; //25 Hz
        case 50:
            return 0x20; //50 Hz
        case 100:
            return 0x30; //100 Hz
        case 200:
            return 0x40;//200 Hz
        case 400:
            return 0x50;//400 Hz
        case 800:
            return 0x60;//800 Hz
        default:
            return 0x30;//Default 100 Hz
    }
}

uint8_t L3G4200D::getScale(uint16_t scale) {
    switch (scale) {
        case 250:
            return 0x00;//±250dps
        case 500:
            return 0x10;//±500dps
        case 2000:
            return 0x20;//±2000dps
        default:
            return 0x00; //Default ±250dps
    }
}

bool L3G4200D::begin() {
    Wire.begin();
    Wire.beginTransmission(address);
    Wire.write(CTRL_REG1);
    Wire.write(0x0F | odr);
    Wire.endTransmission();

    Wire.beginTransmission(address);
    Wire.write(CTRL_REG4);
    Wire.write(scale);
    Wire.endTransmission();

    Wire.beginTransmission(address);
    Wire.write(CTRL_REG1);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)address, (uint8_t)1);
    if (Wire.available()) {
        return (Wire.read() == (0x0F | odr));
    }
    return false;
}

bool L3G4200D::lerTudo() {
    bool verificador = true;

    Wire.beginTransmission(address);
    Wire.write(OUT_X_L | 0x80);
    verificador = verificador && (Wire.endTransmission() == 0);

    Wire.requestFrom((uint8_t)address, (uint8_t)6);
    if (Wire.available() == 6) {
        uint8_t xlo = Wire.read();
        uint8_t xhi = Wire.read();
        uint8_t ylo = Wire.read();
        uint8_t yhi = Wire.read();
        uint8_t zlo = Wire.read();
        uint8_t zhi = Wire.read();

        x = (int16_t)(xlo | (xhi << 8));
        y = (int16_t)(ylo | (yhi << 8));
        z = (int16_t)(zlo | (zhi << 8));
    }
    return verificador;
}

