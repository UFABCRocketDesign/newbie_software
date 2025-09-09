#ifndef _Paraquedas_h
#define _Paraquedas_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Paraquedas {
private:
  const int ALT_PARAQUEDAS;
  const int pino;
  bool ativado = false;
  const bool apogeuApenas;
  unsigned long atraso;
  unsigned long tempoAtivacao;
  unsigned long futureMillis = 0;
  bool paraquedasAtivado = false;
  bool intervaloIniciado = false;
public:
  // Construtor
  Paraquedas(int pino, int altParaquedas, unsigned long atraso, unsigned long tempoAtivacao);

  // Método para inicializar o pino do paraquedas no setup
  void iniciar();

  // Método para gerenciar a ativação dos paraquedas
  void gerenciar(bool apogeuAtingido, float mediaAltitudeFiltrada, unsigned long currentMillis);

  // Método para verificar o estado do paraquedas e printar
  bool getParaquedas();
};

#endif
