#include "Sensores.h"

Sensor::Sensor(uint8_t endereco, long recalibracao) : address(endereco), recalTime(recalibracao) {

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
