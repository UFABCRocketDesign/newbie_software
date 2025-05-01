class Filtro {
private:
  const unsigned int numLeituras;
  float *leituras = new float[numLeituras];
  float total = 0;
  int indiceAtual = 0;
public:
  Filtro(unsigned int n);
  float filtro(float altitudeReal);
  float getMedia();
};