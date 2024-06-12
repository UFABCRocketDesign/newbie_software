#ifndef _Filtro_h
#define _Filtro_h

class Filtro {
private:
  const int NUM_LEITURAS = 10;
  float *leituras;
  float somaLeituras = 0;
  int indiceLeitura = 0;

public:
  // Construtor
  Filtro();

  // Destrutor
  ~Filtro();

  float atualizarFiltro(float novaLeitura);
  float getFiltro();
};

#endif
