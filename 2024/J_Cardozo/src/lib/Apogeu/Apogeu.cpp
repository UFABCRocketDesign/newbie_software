#include "Apogeu.h"
#include <stdlib.h> // Para malloc e free

Apogeu::Apogeu(int tamanho, float fator)
  : historicoTamanho(tamanho), fatorDescida(fator) {
  // Aloca memória dinamicamente para o array historico
  historico = (float*)malloc(historicoTamanho * sizeof(float));
  if (historico == NULL) {
    // Tratamento de erro se a alocação falhar
    // Aqui você pode lançar uma exceção ou lidar de outra forma com o erro
  }
  for (int i = 0; i < historicoTamanho; i++) {
    historico[i] = 0; // Inicializa o array com zeros
  }
}

Apogeu::~Apogeu() {
  // Libera a memória alocada dinamicamente
  free(historico);
}

bool Apogeu::deteccaoApogeu(float entrada) {
  historico[indice] = entrada;
  int contadorHistorico = 0;
  if (++indice >= historicoTamanho) {
    indice = 0;
  }

  for (int i = 1; i < historicoTamanho; i++) {
    if (historico[(indice + i - 1) % historicoTamanho] > historico[(indice + i) % historicoTamanho]) {
      contadorHistorico++;
    }
  }

  estaDescendo = false;

  if (contadorHistorico >= fatorDescida * historicoTamanho) {
    estaDescendo = true;
  }
  return estaDescendo;
}

bool Apogeu::getEstaDescendo() const {
  return estaDescendo;
}
