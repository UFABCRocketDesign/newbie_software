#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp; // Declaração da biblioteca
#define filt_i 10
#define filt_f 20

float nova_altLeitura, cont_sub, cont_subidas, cont_desc, ult_subida;
float altura_inicio, media_alt_inicio;
int j, i;
float list_media_movel[filt_i], media_movel, nova_media_movel, antiga_media_movel;
float list_media_movel_lg[filt_f], media_movel_lg, nova_media_movel_lg;

// the setup function runs once when you press reset or power the board
void setup() {
  // Inicializando o led embutido no arduino
  pinMode(LED_BUILTIN, OUTPUT);
  
  // INICIALIZA O MONITOR SERIAL E PUXA BIBLIOTECA
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  //Serial.println("Temperature (ºC)\tPressure (Pa)\tAltitude(m)\tPressure at sealevel (calculated) (Pa)\tReal altitude (m) ");
  Serial.println("Altitude\tMedia Movel(10)");
  
  // Medicao
  for (j=0; j<10; j++) {
    altura_inicio = bmp.readAltitude();
    media_alt_inicio = media_alt_inicio + altura_inicio;
    delay (100);
    list_media_movel[j] = altura_inicio;
  }
  media_alt_inicio = media_alt_inicio / 10;

  for (j=0; j<10; j++) {
    list_media_movel[j] = list_media_movel[j] - media_alt_inicio;
    list_media_movel_lg[j] = list_media_movel[j] - media_alt_inicio;
  }
  i = filt_i;

  // Primeira media movel
  for (j=0; j<filt_i; j++) {
      media_movel = media_movel + list_media_movel[j];
  }
  media_movel = media_movel / filt_i;

  // Inicialização de variáveis
  nova_altLeitura = bmp.readAltitude() - media_alt_inicio;
  cont_sub = 0; 
  cont_subidas = 0;
  cont_desc = 0;
  ult_subida = 0;
  altura_inicio = 0;
  media_movel = 0;
  nova_media_movel = 0;
  antiga_media_movel = 0;
  media_movel_lg = 0;
  nova_media_movel_lg = 0;

}

// the loop function runs over and over again forever
void loop() {
    media_movel = 0;
    media_movel_lg = 0;
    
    // Zerar contagem de altitude
    if (cont_sub > 0 and cont_desc == 1) {
      cont_sub = 0;
    }
    if (cont_desc > 0 and cont_sub == 1) {
      cont_desc = 0;
    }
    
    // Detecção de Apogeu
    nova_altLeitura = bmp.readAltitude() - media_alt_inicio;
    
    antiga_media_movel = nova_media_movel;
    // Media Movel - 10 e 20
      // Mudança do vetor, considerando 10 valores mais recentes
    for (j=0; j<9; j++) {
      list_media_movel[j] = list_media_movel[j+1];
    }
    list_media_movel[filt_i-1] = nova_altLeitura;
      // Cálculo da Média Movel
    for (j=0; j<filt_i; j++) {
      media_movel = media_movel + list_media_movel[j];
    }
    nova_media_movel = media_movel / filt_i;

      // Mudança do vetor, considerando 20 valores mais recentes
    if (i < filt_f) {
      list_media_movel_lg[i] = nova_altLeitura;
      i += 1;
    }
    if (i = filt_f) {
      for (j=0; j<filt_f; j++) {
        list_media_movel_lg[j] = list_media_movel_lg[j+1];
      }
      list_media_movel_lg[filt_f-1] = nova_altLeitura;
    
        // Cálculo da Média Movel
      for (j=0; j<filt_f; j++) {
        media_movel_lg = media_movel_lg + list_media_movel_lg[j];
      }
      nova_media_movel_lg = media_movel_lg / filt_f;
    }

    // Consideração de Subidas e Descidas
    if (nova_media_movel > antiga_media_movel) {
      cont_sub += 1;
    }
    else if (nova_media_movel < antiga_media_movel) {
      cont_desc += 1;
    }

        // Altura
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(nova_altLeitura);
    Serial.print("\t");
    Serial.print(nova_media_movel);
    Serial.print("\t");
    Serial.print(nova_media_movel_lg);

    // Identificação de subida/descida/apogeu
    if (cont_sub > 10) {
      Serial.print("\tSubindo");
      cont_subidas = 1;
      ult_subida = nova_altLeitura;
    }
    else if (cont_desc > 10) {
      Serial.print("\tDescendo");
    }
    if (cont_subidas > 0 and cont_desc == 10) {
      Serial.print("\tApogeu em: ");
      Serial.print(ult_subida);
    }

    Serial.println();
    
}
