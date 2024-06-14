#ifndef PARAQUEDAS_H
#define PARAQUEDAS_H

class Paraquedas {
  const int intervaloLigado;
  bool paraquedas = false;
  bool paraquedasData = false;
  unsigned long paraquedasTempo = 0;
  const int ign;
  float delay;
  float altura;

public:
  Paraquedas(int tempoLigado, float tempoDelay, int portaIgn, float alturaAtivacao);
  
  void begin();
  void ativarParaquedas(float alturaAtual, unsigned long currentTime, bool estaDescendo);
  bool getData() const;
};

#endif
