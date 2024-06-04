#ifndef FILTROS_H
#define FILTROS_H

class FiltroMediaMovel {
  const int numLeituras;
  float* dados;
  int indice = 0;
  float media = 0;

public:
  FiltroMediaMovel(int tamanho = 10);
  ~FiltroMediaMovel();

  float aplicarFiltro(float entrada);
  float getMedia() const;
  int getNumLeituras() const;
};

#endif
