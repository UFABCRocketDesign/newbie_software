#include "HMC5883L.h"

HMC5883L::HMC5883L(uint8_t samples, uint16_t gain, uint8_t mode) : x(0), y(0), z(0) {
    configA = (getSamples(samples) << 5) | 0x18;
    configB = getGain(gain);
    this->mode = getMode(mode);
}

uint8_t HMC5883L::getGain(uint16_t gain) {
    switch (gain) {
        case 1370:
            return 0x00; // ±0.88 Ga
        case 1090:
            return 0x20; // ±1.3 Ga
        case 820:
            return 0x40; // ±1.9 Ga
        case 660:
            return 0x60; // ±2.5 Ga
        case 440:
            return 0x80; // ±4.0 Ga
        case 390:
            return 0xA0; // ±4.7 Ga
        case 330:
            return 0xC0; // ±5.6 Ga
        case 230:
            return 0xE0; // ±8.1 Ga
        default:
            return 0x20; // Default ±1.3 Ga (1090 LSB/Ga)
    }
}

uint8_t HMC5883L::getSamples(uint8_t samples) {
    switch (samples) {
        case 1:
            return 0x00; // 1 sample 
        case 2:
            return 0x01; // 2 samples 
        case 4:
            return 0x02; // 4 samples 
        case 8:
            return 0x03; // 8 samples 
        default:
            return 0x03; // Default 8 samples
    }
}

uint8_t HMC5883L::getMode(uint8_t mode) {
    switch (mode) {
        case 0:
            return 0x00; // Continuous-measurement mode
        case 1:
            return 0x01; // Single-measurement mode
        case 2:
            return 0x02; // Idle mode
        default:
            return 0x00; // Default continuous-measurement mode
    }
}

bool HMC5883L::begin() {
    Wire.begin();

    Wire.beginTransmission(HMC5883L_Address);
    Wire.write(0x00);
    Wire.write(configA);
    Wire.endTransmission();

    Wire.beginTransmission(HMC5883L_Address);
    Wire.write(0x01);
    Wire.write(configB);
    Wire.endTransmission();

    Wire.beginTransmission(HMC5883L_Address);
    Wire.write(0x02);
    Wire.write(mode);
    Wire.endTransmission();

    Wire.beginTransmission(HMC5883L_Address);
    Wire.write(0x02);
    Wire.endTransmission();
    Wire.requestFrom(HMC5883L_Address, 1);
    if (Wire.available()) {
        return (Wire.read() == mode);
    }
    return false;
}

void HMC5883L::lerTudo() {
    Wire.beginTransmission(HMC5883L_Address);
    Wire.write(0x03); 
    Wire.endTransmission();
    Wire.requestFrom(HMC5883L_Address, 6);
    if (Wire.available() == 6) {
        x = Wire.read() << 8; 
        x |= Wire.read();    
        z = Wire.read() << 8;
        z |= Wire.read();     
        y = Wire.read() << 8; 
        y |= Wire.read();    
    }
}

float HMC5883L::getX() {
    return x;
}

float HMC5883L::getY() {
    return y;
}

float HMC5883L::getZ() {
    return z;
}
