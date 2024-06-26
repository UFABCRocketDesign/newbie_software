#include "BMP085.h"

BMP085::BMP085(long recalibrationValue) : Sensor(BMP085_ADDRESS, recalibrationValue) {

}

bool BMP085::begin() {
    Wire.beginTransmission(address);
    Wire.write(0XAA);
    if (Wire.endTransmission() != 0) {
        return false; 
    }

    Wire.requestFrom((uint8_t)address, (uint8_t)22);
    unsigned long tempo = micros();
    while (Wire.available() < 22) {
        if (tempo + 10 < micros()) {
            return false; 
        }
    }

    ac1 = Wire.read() << 8 | Wire.read();
    ac2 = Wire.read() << 8 | Wire.read();
    ac3 = Wire.read() << 8 | Wire.read();
    ac4 = Wire.read() << 8 | Wire.read();
    ac5 = Wire.read() << 8 | Wire.read();
    ac6 = Wire.read() << 8 | Wire.read();
    b1 = Wire.read() << 8 | Wire.read();
    b2 = Wire.read() << 8 | Wire.read();
    mb = Wire.read() << 8 | Wire.read();
    mc = Wire.read() << 8 | Wire.read();
    md = Wire.read() << 8 | Wire.read();

    return true; 
}

bool BMP085::lerTudo(float pressaoInicial) {
    bool verificador = true;
    thisRead = millis();

    //Calibracao T
    Wire.beginTransmission(address);
    Wire.write(0xF4);
    Wire.write(0x2E);
    verificador = verificador && (Wire.endTransmission() == 0);
    

    delayMicroseconds(4550);

    Wire.beginTransmission(address);
    Wire.write(0xF6);
    verificador = verificador && (Wire.endTransmission() == 0);


    Wire.requestFrom((uint8_t)address, (uint8_t)2);
    unsigned long tempo = micros();
    while (Wire.available() < 2)
    {
        if (tempo + 10 < micros())break;
    }
    ut = Wire.read() << 8 | Wire.read();

    //Calibracao P
    Wire.beginTransmission(address);
    Wire.write(0xF4);
    Wire.write(0x34 + (oss << 6));
    verificador = verificador && (Wire.endTransmission() == 0);

    delay(2 + (3 << oss));

    Wire.beginTransmission(address);
    Wire.write(0xF6);
    verificador = verificador && (Wire.endTransmission() == 0);


    Wire.requestFrom((uint8_t)address, (uint8_t)3);
    tempo = micros();
    while (Wire.available() < 3)
    {
        if (tempo + 10 < micros())break;
    }

    up = (((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | (unsigned long)Wire.read()) >> (8 - oss);

    //Calculo temperatura verdadeira
    x1 = (ut - (long)ac6) * (long)ac5 >> 15;
    x2 = ((long)mc << 11) / (x1 + md);
    b5 = x1 + x2;
    temperatura = (float)((b5 + 8) >> 4)/10;

    //Calculo pressao verdadeira
    b6 = b5 - 4000;
    x1 = (b2 * (b6 * b6 >> 12)) >> 11;
    x2 = (ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = ((((long)ac1 * 4 + x3) << oss) + 2) >> 2;

    x1 = (ac3 * b6) >> 13;
    x2 = (b1 * (b6 * b6 >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

    b7 = ((unsigned long)(up - b3) * (50000 >> oss));
    if (b7 < 0x80000000) {
        pressao = (b7 << 1) / b4;
    } else {
        pressao = (b7 / b4) << 1;
    }

    x1 = (pressao >> 8) * (pressao >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * pressao) >> 16;
    pressao += (x1 + x2 + 3791) >> 4;

    //Calculo da altitude(absoluta ou relativa)
    altitude = 44330 * (1.0 - pow((pressao / (pressaoInicial/100)), (1/5.255)));

    if (verificador) {
        lastRead = thisRead;
    }
    recalibrar("BMP");
    return verificador;
}

bool BMP085::lerTudo() {
    return this->lerTudo(101325.0);
}

float BMP085::getTemperatura() {
    return temperatura;
}

long BMP085::getPressao() {
    return pressao;
}

float BMP085::getAltitude() {
    return altitude;
}