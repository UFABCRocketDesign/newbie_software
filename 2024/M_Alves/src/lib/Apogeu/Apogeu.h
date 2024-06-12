#ifndef _Apogeu_h
#define _Apogeu_h

class Apogeu {
private:
  float alturaAnterior = -1;
  int contador = 0;
  int estado = 0;  // estado 0 -> subindo; estado 1 -> descendo
  bool apogeu = false;

public:
  // Método para detectar o apogeu
  bool detectar(float alturaAtual);
  bool getApogeu();
};
#endif