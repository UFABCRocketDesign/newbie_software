#include "Apogeu.h"

int Apogeu::detectorQueda(float altura) {
  float altitudeAnterior = 0;

  if (altura < altitudeAnterior) {
    contadorQueda++;
  } else {
    contadorQueda = 0;
  }

  altitudeAnterior = altura;

  if (contadorQueda >= 10) {
    queda = 1;
  } else {
    queda = 0;
  }
  return queda;
}

int Apogeu::getValor() {
  return queda;
}