#include "Paraquedas.h"
#include <Arduino.h>

Paraquedas::Paraquedas(int tempoLigado, float tempoDelay, int portaIgn, float alturaAtivacao)
  : intervaloLigado(tempoLigado), delay(tempoDelay), ign(portaIgn), altura(alturaAtivacao) {
}

void Paraquedas::begin() {
  pinMode(ign, OUTPUT);
}

void Paraquedas::ativarParaquedas(float alturaAtual, unsigned long currentTime, bool estaDescendo) {
  if (delay == 0) {
    if (altura == 0) {
      if (estaDescendo && !paraquedas) {
        paraquedas = true;
        paraquedasTempo = millis();
        paraquedasData = true;
      }

      if (paraquedas && currentTime >= paraquedasTempo + intervaloLigado) {
        paraquedasData = false;
      }
    } else {
      if (estaDescendo && !paraquedas) {
        paraquedas = true;
      }
      if (paraquedas && alturaAtual <= altura && paraquedasTempo == 0) {
        paraquedasTempo = millis();
        paraquedasData = true;
      }
      if (paraquedas && paraquedasTempo != 0 && currentTime >= paraquedasTempo + intervaloLigado) {
        paraquedasData = false;
      }
    }
  } else {
    if (altura == 0) {
      if (estaDescendo && !paraquedas) {
        paraquedas = true;
        paraquedasTempo = millis();
      }
      if (paraquedas && currentTime >= paraquedasTempo + delay && currentTime < paraquedasTempo + delay + intervaloLigado) {
        paraquedasData = true;
      } else if (paraquedas && currentTime >= paraquedasTempo + delay + intervaloLigado) {
        paraquedasData = false;
      }
    } else {
      if (estaDescendo && !paraquedas) {
        paraquedas = true;
      }
      if (paraquedas && alturaAtual <= altura && paraquedasTempo == 0) {
        paraquedasTempo = millis();
      }
      if (paraquedas && paraquedasTempo != 0 && currentTime >= paraquedasTempo + delay && currentTime < paraquedasTempo + delay + intervaloLigado) {
        paraquedasData = true;
      } else if (paraquedas && currentTime >= paraquedasTempo + delay + intervaloLigado) {
        paraquedasData = false;
      }
    }
  }
  if (paraquedasData == true) {
    digitalWrite(ign, HIGH);
  } else {
    digitalWrite(ign, LOW);
  }
}

bool Paraquedas::getData() const {
  return paraquedasData;
}
