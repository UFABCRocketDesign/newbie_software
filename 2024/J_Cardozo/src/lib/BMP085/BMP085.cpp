#include "BMP085.h"


#define BMP085_ADDRESS 0x77

#define BMP085_AC1 0xAA

void BMP085::begin() {
    Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(0XAA);
	Wire.endTransmission();

	Wire.requestFrom(BMP085_ADDRESS, (uint8_t)22);
	unsigned long tempo = micros();
	while (Wire.available() < 22)
	{
		if (tempo + 10 < micros())break;
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
}

long BMP085::lerCalibracaoT() {
    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(0xF4);
    Wire.write(0x2E);
    Wire.endTransmission();

    delayMicroseconds(4550);

    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(0xF6);
    Wire.endTransmission();

    Wire.requestFrom(BMP085_ADDRESS, (uint8_t)2);
    unsigned long tempo = micros();
    while (Wire.available() < 2)
    {
        if (tempo + 10 < micros())break;
    }
    ut = Wire.read() << 8 | Wire.read();
    return ut;
}

long BMP085::lerCalibracaoP() {
    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(0xF4);
    Wire.write(0x34 + (oss << 6));
    Wire.endTransmission();

    delay(2 + (3 << oss));

    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(0xF6);
    Wire.endTransmission();

    Wire.requestFrom(BMP085_ADDRESS, (uint8_t)3);
    unsigned long tempo = micros();
    while (Wire.available() < 3)
    {
        if (tempo + 10 < micros())break;
    }

    up = (((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | (unsigned long)Wire.read()) >> (8 - oss);
    return up;
}

void BMP085::lerTudo(float alturaInicial = 1013.25) {
    ut = lerCalibracaoT();
    up = lerCalibracaoP();

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
    altura = 44330 * (1.0 - pow((pressao / (alturaInicial/100)), (1/5.255)));
}

float BMP085::getTemperatura() {
    return temperatura;
}

long BMP085::getPressao() {
    return pressao;
}

float BMP085::getAltitude() {
    return altura;
}