#include "Paraquedas.h"

Paraquedas::Paraquedas(int pino, int altParaquedas, unsigned long atraso, unsigned long tempoAtivacao)
  : pino(pino), ALT_PARAQUEDAS(altParaquedas), apogeuApenas(altParaquedas == 0), atraso(atraso), tempoAtivacao(tempoAtivacao) {
}

void Paraquedas::iniciar() {
  pinMode(pino, OUTPUT);
  digitalWrite(pino, LOW);
}

void Paraquedas::gerenciar(bool apogeuAtingido, float mediaAltitudeFiltrada, unsigned long currentMillis) {
  if (apogeuAtingido && !ativado && !paraquedasAtivado && (apogeuApenas || mediaAltitudeFiltrada < ALT_PARAQUEDAS)) {
    if (!intervaloIniciado) {
      futureMillis = currentMillis + atraso;
      intervaloIniciado = true;
    }
    if (currentMillis >= futureMillis) {
      digitalWrite(pino, HIGH);
      ativado = true;
      futureMillis = currentMillis + tempoAtivacao;
    }
  } else if (ativado && currentMillis >= futureMillis) {
    digitalWrite(pino, LOW);
    ativado = false;
    paraquedasAtivado = true;
  }
}

bool Paraquedas::getParaquedas() {
  return ativado;
}
