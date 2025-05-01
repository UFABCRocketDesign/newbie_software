#include "Paraquedas.h"

int Paraquedas::ativar(float altura, int queda) {
  if (queda == 1 && estadoParaquedas == 0 && (!usaAltura || altura < altitudeTeto)) {
    estadoParaquedas = 1;
    timer = millis();
  }

  if (estadoParaquedas == 1 && (millis() - timer) >= tempoAtraso) {
    estadoParaquedas = 2;
    digitalWrite(pinoIgnicao, HIGH);
    desativacao = millis();
  }

  if (estadoParaquedas == 2 && (millis() - desativacao) >= tempoLigado) {
    estadoParaquedas = 3;
    digitalWrite(pinoIgnicao, LOW);
  }
}

int Paraquedas::getValor() {
  return estadoParaquedas;
}