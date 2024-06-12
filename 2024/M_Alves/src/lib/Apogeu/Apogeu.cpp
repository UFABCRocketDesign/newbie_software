#include "Apogeu.h"

bool Apogeu::detectar(float alturaAtual) {
  if (alturaAnterior != -1 && alturaAtual < alturaAnterior) {
    contador++;
    if (contador >= 25) {
      estado = 1;
      if (estado == 1 && !apogeu) {
        apogeu = true;
      }
    }
  } else {
    contador = 0;
    estado = 0;
  }

  alturaAnterior = alturaAtual;  // Atualize a altitude anterior para a próxima iteração
  return apogeu;
}

bool Apogeu::getApogeu() {
  return apogeu;
}
