#ifndef _Apogeu_h
#define _Apogeu_h

class Apogeu {
private:
  int queda = 0;
  int contadorQueda = 0;
public:
  int detectorQueda(float altura);
  int getQueda();
};
#endif