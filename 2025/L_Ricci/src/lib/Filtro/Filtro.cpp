#include "Filtro.h"

Filtro::Filtro(unsigned int n) : numLeituras(n) {
  for (unsigned int i = 0; i < numLeituras; i++) leituras[i] = 0;
}

Filtro::~Filtro() {
  delete[] leituras;
}

float Filtro::filtro(float altitudeReal) {
  total = total - leituras[indiceAtual];
  leituras[indiceAtual] = (altitudeReal);
  total = total + leituras[indiceAtual];
  indiceAtual = (indiceAtual + 1) % numLeituras;
  media = total / numLeituras;
  return media;
}

float Filtro::getMedia() {
  return media;
}