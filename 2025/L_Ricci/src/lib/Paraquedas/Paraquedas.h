#ifndef _Paraquedas_h
#define _Paraquedas_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Paraquedas {
private:
  int pinoIgnicao;
  int estadoParaquedas;
  int altitudeTeto;
  unsigned long timer;
  unsigned long desativacao;
  const unsigned long tempoAtraso;
  const unsigned long tempoLigado;
  const bool usaAltura;

public:
  Paraquedas(int pino, unsigned long atraso, unsigned long ligado, bool altura, int altitudeTeto)
    : pinoIgnicao(pino), tempoAtraso(atraso), tempoLigado(ligado), usaAltura(altura), altitudeTeto(altitudeTeto) {}
  int ativar(float altura, int queda);
  int getValor();
};
#endif