#ifndef PARAQUEDAS_H
#define PARAQUEDAS_H

class Paraquedas {
  const int intervaloLigado;
  bool paraquedas = false;
  bool paraquedasData = false;
  unsigned long paraquedasTempo = 0;
  const int ign;
  const int saude;
  float delay;
  float altura;
  int sensibilidadeSaude;

public:
  Paraquedas(int tempoLigado, float tempoDelay, int portaIgn, int portaSaude, float alturaAtivacao, int sensibilidade = 100);
  
  void begin();
  void ativarParaquedas(float alturaAtual, unsigned long currentTime, bool estaDescendo, bool estaDescendoTimer);
  bool getData() const;
  bool getInfo();
};

#endif
