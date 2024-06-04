#include "Filtros.h"
#include <stdlib.h>

FiltroMediaMovel::FiltroMediaMovel(int tamanho)
  : numLeituras(tamanho) {
  dados = (float*)malloc(numLeituras * sizeof(float));
  for (int i = 0; i < numLeituras; i++) {
    dados[i] = 0;
  }
}

FiltroMediaMovel::~FiltroMediaMovel() {
  free(dados);
}

float FiltroMediaMovel::aplicarFiltro(float entrada) {
  float soma = 0;
  dados[indice] = entrada;
  for (int i = 0; i < numLeituras; i++) {
    soma += dados[i];
  }
  media = soma / numLeituras;
  indice++;
  if (indice >= numLeituras) {
    indice = 0;
  }
  return media;
}

float FiltroMediaMovel::getMedia() const {
  return media;
}

int FiltroMediaMovel::getNumLeituras() const {
  return numLeituras;
}
