#include "Paraquedas.h"

void Paraquedas::begin() {
  pinMode(pinoIgnicao, OUTPUT);
  pinMode(pinoSaude, INPUT);
}

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

int Paraquedas::getHealth() {
  return analogRead(pinoSaude) < sensibilidadeSaude;
}

void Paraquedas::emergency(bool estadoEmergencia, unsigned long tempoEmergencia) {
  if (estadoEmergencia) {
    usaAlturaEM = usaAltura;
    tempoAtrasoEM = tempoAtraso;
    tempoAtraso = (unsigned long)(tempoEmergencia * 1000);
    usaAltura = false;
    emergencia = true;
  } else if (emergencia){
    usaAltura = usaAlturaEM;
    tempoAtraso = tempoAtrasoEM;
    emergencia = false;
  }
}

void Paraquedas::imprimirTudo() {
  Serial.println(usaAltura);
  Serial.println(tempoAtraso);
}