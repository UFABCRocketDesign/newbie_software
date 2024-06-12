#include "Filtro.h"

Filtro::Filtro() {
  leituras = new float[NUM_LEITURAS];
  for (int i = 0; i < NUM_LEITURAS; i++) {
    leituras[i] = 0;
  }
}

Filtro::~Filtro() {
  delete[] leituras;
}

float Filtro::atualizarFiltro(float novaLeitura) {
  somaLeituras -= leituras[indiceLeitura];
  leituras[indiceLeitura] = novaLeitura;
  somaLeituras += novaLeitura;
  indiceLeitura = (indiceLeitura + 1) % NUM_LEITURAS;
  return somaLeituras / NUM_LEITURAS;
}

float Filtro::getFiltro() {
  return somaLeituras / NUM_LEITURAS;
}
