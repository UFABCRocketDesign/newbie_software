#include "Sensores.h"

Sensor::Sensor(uint8_t endereco, int recalibracao, int timer) : address(endereco), recalTime(recalibracao), timerTime(timer) {

}

long Sensor::getTime() {
    return (thisRead - lastRead);
}

void Sensor::recalibrar() {
    if (getTime() > recalTime) {
        begin();
        Serial.println("begin");
    }
}

void Sensor::timer() {
    estaDescendoTimer = false;
    if (getTime() > timerTime) {
        estaDescendoTimer = true;
    }
}

bool Sensor::getEstaDescendoTimer() {
    return estaDescendoTimer;
}
