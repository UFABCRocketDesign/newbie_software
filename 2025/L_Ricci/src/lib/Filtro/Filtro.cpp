#include "Filtro.h"

FiltroMediaMovel::FiltroMediaMovel(unsigned int n) : numLeituras(n) {}

float FiltroMediaMovel::filtro(float altitudeReal) {
  total = total - leituras[indiceAtual];
  leituras[indiceAtual] = (altitudeReal);
  total = total + leituras[indiceAtual];
  indiceAtual = (indiceAtual + 1) % numLeituras;
  float altura = total / numLeituras;

  return altura;
}

float FiltroMediaMovel::getMedia() {
  return (total / numLeituras);
}