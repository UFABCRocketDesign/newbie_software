#ifndef ACEL_H
#define ACEL_H

class Apogeu {
  const int historicoTamanho;
  const float fatorDescida;
  float* historico;
  int indice = 0;
  bool estaDescendo;

public:
  Apogeu(int tamanho = 20, float fator = 0.85f);
  ~Apogeu();

  bool deteccaoApogeu(float entrada);
  bool getEstaDescendo() const;
};

#endif
