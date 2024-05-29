#include "bibliotecaBMP085.h"


#define BMP085_ADDRESS 0x77

#define BMP085_AC1 0xAA
#define BMP085_AC2 0xAC
#define BMP085_AC3 0xAE
#define BMP085_AC4 0xB0
#define BMP085_AC5 0xB2
#define BMP085_AC6 0xB4
#define BMP085_B1 0xB6
#define BMP085_B2 0xB8
#define BMP085_MB 0xBA
#define BMP085_MC 0xBC
#define BMP085_MD 0xBE

uint32_t BMP085::read(uint8_t reg, uint8_t bytes) {
    if (bytes < 2 || bytes > 3) {
        return 0;
    }

    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(BMP085_ADDRESS, bytes);

    if (Wire.available() >= bytes) {
        uint32_t value = 0;
        for (int i = 0; i < bytes; i++) {
            value = (value << 8) | Wire.read();
        }
        return value;
    } else {
        return 0;
    }
}

void BMP085::write8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

void BMP085::begin() {
    ac1 = read(BMP085_AC1, 2);
    ac2 = read(BMP085_AC2, 2);
    ac3 = read(BMP085_AC3, 2);
    ac4 = read(BMP085_AC4, 2);
    ac5 = read(BMP085_AC5, 2);
    ac6 = read(BMP085_AC6, 2);
    b1 = read(BMP085_B1, 2);
    b2 = read(BMP085_B2, 2);
    mb = read(BMP085_MB, 2);
    mc = read(BMP085_MC, 2);
    md = read(BMP085_MD, 2);
}

float BMP085::lerCalibracaoT() {
    write8(0xF4, 0x2E);
    delay(5);  // 4.5ms delay
    return read(0xF6, 2);
}

float BMP085::lerCalibracaoP() {
    write8(0xF4, (0x34 + (oss << 6)));
    delay(2 + (3 << oss));
    return read(0xF6, 3) >> (8 - oss);
}

void BMP085::lerTudo() {
    ut = lerCalibracaoT();
    x1 = (ut - (int32_t)ac6) * (int32_t)ac5 >> 15;
    x2 = ((int32_t)mc << 11) / (x1 + md);
    b5 = x1 + x2;
    t = (b5 + 8) >> 4;

    up = lerCalibracaoP();
    b6 = b5 - 4000;
    x1 = (b2 * (b6 * b6 >> 12)) >> 11;
    x2 = (ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = ((((int32_t)ac1 * 4 + x3) << oss) + 2) >> 2;

    x1 = (ac3 * b6) >> 13;
    x2 = (b1 * (b6 * b6 >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;

    b7 = ((uint32_t)(up - b3) * (50000 >> oss));
    if (b7 < 0x80000000) {
        p = (b7 << 1) / b4;
    } else {
        p = (b7 / b4) << 1;
    }

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p += (x1 + x2 + 3791) >> 4;

    h = 44330 * (1.0 - pow((p / 101325.0), 0.1903));
}

float BMP085::getTempemperatura() {
    return t / 10.0;
}

int32_t BMP085::getPressao() {
    return p;
}

float BMP085::getAltitude() {
    return h;
}
