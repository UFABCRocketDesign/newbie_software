#ifndef _Paraquedas_h
#define _Paraquedas_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
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
  unsigned long tempoAtraso;
  unsigned long tempoAtrasoEM;
  unsigned long tempoLigado;
  bool usaAltura;
  bool usaAlturaEM;
  bool emergencia = false;
  const int pinoIgnicao;
  const int pinoSaude;

public:
  Paraquedas(int pino, int saude, unsigned long atraso, unsigned long ligado, bool altura, int altitudeTeto, int sensibilidade = 150)
    : pinoIgnicao(pino), pinoSaude(saude), tempoAtraso(atraso), tempoLigado(ligado), usaAltura(altura), altitudeTeto(altitudeTeto), sensibilidadeSaude(sensibilidade) {}
  int ativar(float altura, int queda);
  int getValor();
  int getHealth();
  void emergency(bool estadoEmergencia, unsigned long atrasoEmergencia = 0);
};
#endif