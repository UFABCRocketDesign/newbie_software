#ifndef _Paraquedas_h
#define _Paraquedas_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Paraquedas {
private:
  int estadoParaquedas;
  int altitudeTeto;
  int sensibilidadeSaude;
  unsigned long timer;
  unsigned long desativacao;
  const int pinoIgnicao;
  const int pinoSaude;
  const unsigned long tempoAtraso;
  const unsigned long tempoLigado;
  const bool usaAltura;

public:
  Paraquedas(int pino, int saude, unsigned long atraso, unsigned long ligado, bool altura, int altitudeTeto, int sensibilidade = 150)
    : pinoIgnicao(pino), pinoSaude(saude), tempoAtraso(atraso), tempoLigado(ligado), usaAltura(altura), altitudeTeto(altitudeTeto), sensibilidadeSaude(sensibilidade) {}
  int ativar(float altura, int queda);
  int getValor();
  int getHealth();
};
#endif