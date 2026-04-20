#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
////---VARIAVEIS--------------------------------------------------------------------------------
//---Calibracao---------------------------------------------------------------------------------
const int amostra_calibracao= 100;       //manual
float altitude_zero = 0.0f;               //auto
float soma_calibracao = 0;                //auto
//filtro_movel----------------------------------------------------------------------------------
const int n_amostra_filtro = 10;          //manual
float altura_bruta = 0;                   //auto
float readings[n_amostra_filtro];         //auto
float valor_medio_filtromovel = 0;        //auto
float leitura_n_filtromovel = 0;          //auto
//filtro_alpha----------------------------------------------------------------------------------
float alpha = 0.1;                        //manual (0 < alpha < 1)
float valor_filtro_alpha = 0;             // auto
int readIndex = 0;                        //auto
//detectar_queda--------------------------------------------------------------------------------
const float velocidade_queda_max = -2.0;  //manual
const int n_amostra_confirmar_queda = 3;  //manual
int amostras_confirmada = 0;              //auto
float altura_anterior = 0;                //auto
float velocidade = 0;                     //auto
float tempo_ant = 0;                      //auto
bool queda = false;                       //auto
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (int i = 0; i < amostra_calibracao; i++) {
    soma_calibracao += bmp.readAltitude();
  }
  for (int i = 0; i < n_amostra_filtro; i++) {
    readings[i] = 0;
  }
  altitude_zero = soma_calibracao / amostra_calibracao;
  Serial.println("Temperature(*C)\tPressure(Pa)\tAltitude(meters)\tPressure at sealevel (calculated)(Pa)\tReal altitude(meters)");
}


void loop() {
  altura_bruta = bmp.readAltitude() - altitude_zero;
  leitura_n_filtromovel = leitura_n_filtromovel - readings[readIndex];

  // Read the sensor
  readings[readIndex] = altura_bruta;

  // Add the reading to the leitura_n_filtromovel
  leitura_n_filtromovel = leitura_n_filtromovel + readings[readIndex];

  // Advance to the next position in the array
  readIndex = readIndex + 1;

  // If we're at the end of the array, wrap around to the beginning
  if (readIndex >= n_amostra_filtro) {
    readIndex = 0;
  }

  // Calculate the valor_medio_filtromovel
  valor_medio_filtromovel = leitura_n_filtromovel / n_amostra_filtro;

  valor_filtro_alpha = (alpha * valor_medio_filtromovel + (1 - alpha) * valor_filtro_alpha);

  float temp_agr = millis();

  velocidade = ((valor_filtro_alpha - altura_anterior) / (temp_agr - tempo_ant)) * 1000;

  if (velocidade < velocidade_queda_max) {
    amostras_confirmada = amostras_confirmada + 1;
    if (amostras_confirmada > n_amostra_confirmar_queda) {
      queda = true;
    }
  }

  tempo_ant = temp_agr;

  altura_anterior = valor_filtro_alpha;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(altura_bruta);
  Serial.print("\t");

  Serial.print(valor_medio_filtromovel);
  Serial.print("\t");

  Serial.print(valor_filtro_alpha);
  Serial.print("\t");

  Serial.print(velocidade);
  Serial.print("\t");

  Serial.print(queda);
  Serial.print("\t");

  Serial.println();
}