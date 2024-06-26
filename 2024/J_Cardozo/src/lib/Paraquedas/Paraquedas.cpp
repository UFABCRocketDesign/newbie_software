#include "Paraquedas.h"
#include <Arduino.h>

Paraquedas::Paraquedas(int tempoLigado, float tempoDelay, int portaIgn, int portaSaude, float alturaAtivacao, int sensibilidade = 100)
  : intervaloLigado(tempoLigado), delay(tempoDelay), ign(portaIgn), saude(portaSaude), altura(alturaAtivacao), sensibilidadeSaude(sensibilidade) {
}

void Paraquedas::begin() {
  pinMode(ign, OUTPUT);
  pinMode(saude, INPUT);
}

void Paraquedas::ativarParaquedas(float alturaAtual, unsigned long currentTime, bool estaDescendo, bool estaDescendoTimer) {
  if (delay == 0) {
    if (altura == 0) {
      if ((estaDescendo || estaDescendoTimer) && !paraquedas) {
        paraquedas = true;
        paraquedasTempo = millis();
        paraquedasData = true;
      }

      if (paraquedas && currentTime >= paraquedasTempo + intervaloLigado) {
        paraquedasData = false;
      }
    } else {
      if ((estaDescendo || estaDescendoTimer) && !paraquedas) {
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
      if ((estaDescendo || estaDescendoTimer) && !paraquedas) {
        paraquedas = true;
        paraquedasTempo = millis();
      }
      if (paraquedas && currentTime >= paraquedasTempo + delay && currentTime < paraquedasTempo + delay + intervaloLigado) {
        paraquedasData = true;
      } else if (paraquedas && currentTime >= paraquedasTempo + delay + intervaloLigado) {
        paraquedasData = false;
      }
    } else {
      if ((estaDescendo || estaDescendoTimer) && !paraquedas) {
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

bool Paraquedas::getInfo() {
  return analogRead(saude) > sensibilidadeSaude;
}
