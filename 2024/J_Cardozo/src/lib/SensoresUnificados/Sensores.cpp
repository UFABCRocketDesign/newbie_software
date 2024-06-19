#include "Sensores.h"

Sensor::Sensor(uint8_t endereco, int recalibracao, int timer) : address(endereco), recalTime(recalibracao), timerTime(timer) {

}

long Sensor::getTime() {
    return (thisRead - lastRead);
}

void Sensor::recalibrar(String sensor) {
    if (getTime() > recalTime) {
        bool verificador = begin();
        Serial.print("begin "); Serial.print(sensor); Serial.println(verificador);
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
