#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp; // Declaração da biblioteca
float altitudeLeitura, nova_altLeitura, cont_sub, cont_subidas, cont_desc, ult_subida;
float altura_inicio, media_alt_inicio;
int j;
float list_media_movel[10], media_movel, nova_media_movel;

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
  }

  // Primeira media movel
  for (j=0; j<10; j++) {
      media_movel = media_movel + list_media_movel[j];
  }
  media_movel = media_movel / 10;

  // Inicialização de variáveis
  nova_altLeitura = bmp.readAltitude() - media_alt_inicio;
  ult_subida = 0;
  cont_subidas = 0;
  cont_sub = 0;
  cont_desc = 0;
  media_movel = 0;
  
}

// the loop function runs over and over again forever
void loop() {
    // Zerar contagem de altitude
    if (cont_sub > 0 and cont_desc == 1) {
      cont_sub = 0;
    }
    if (cont_desc > 0 and cont_sub == 1) {
      cont_desc = 0;
    }
    
    // Detecção de Apogeu
    altitudeLeitura = nova_altLeitura;
    nova_altLeitura = bmp.readAltitude() - media_alt_inicio;
    
    media_movel = nova_media_movel;
    // Media Movel
      // Mudança do vetor, considerando 10 valores mais recentes
    for (j=0; j<9; j++) {
      list_media_movel[j] = list_media_movel[j+1];
    }
    list_media_movel[9] = nova_altLeitura;
      // Cálculo da Média Movel
    for (j=0; j<10; j++) {
      media_movel = media_movel + list_media_movel[j];
    }
    nova_media_movel = media_movel / 10;
    
    if (nova_altLeitura > altitudeLeitura && nova_media_movel > media_movel) {
      cont_sub += 1;
    }
    else if (nova_altLeitura < altitudeLeitura && nova_media_movel < media_movel) {
      cont_desc += 1;
    }

    // Identificação de subida/descida/apogeu
    if (cont_sub > 10) {
      Serial.print("Subindo");
      cont_subidas = 1;
      ult_subida = nova_altLeitura;
    }
    else if (cont_desc > 10) {
      Serial.print("Descendo");
    }
    if (cont_subidas > 0 and cont_desc == 10) {
      Serial.print("Apogeu em: ");
      Serial.println(ult_subida);
    }

        // Altura
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(nova_altLeitura);
    Serial.print("\t");
    Serial.println(media_movel);
       
}
