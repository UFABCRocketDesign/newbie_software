#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp; // Declaração da biblioteca
const int chipSelect = 53; // Declaração de CS

#define filt_i 10
#define filt_f 20

float nova_altLeitura, cont_sub, cont_subidas, cont_desc, ult_subida;
float altura_inicio, media_alt_inicio;
int j, i;
float media_movel, nova_media_movel, antiga_media_movel;
float media_movel_lg, nova_media_movel_lg;

float list_med_movel[2][filt_f];

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

  // Verifica se o cartão está presente e pode ser inicializado
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // Determina o nome do FILE
  File dataFile = SD.open("paulo.txt", FILE_WRITE);
  // Inicia inserindo essa informação no FILE nomeado
  if (dataFile) {
    dataFile.println("Altitude\tMedia Movel(10)\tMedia Movel(20)");
    dataFile.close();
    // print to the serial port too:
    Serial.println("Altitude\tMedia Movel(10)\tMedia Movel(20)");
  }
  
  // Medicao
  for (j=0; j<10; j++) {
    altura_inicio = bmp.readAltitude();
    media_alt_inicio = media_alt_inicio + altura_inicio;
    delay (100);
    list_med_movel[0][j] = altura_inicio;
    list_med_movel[1][j] = altura_inicio;
  }
  media_alt_inicio = media_alt_inicio / 10;

  for (j=0; j<10; j++) {
    list_med_movel[0][j] = list_med_movel[0][j] - media_alt_inicio;
    list_med_movel[1][j] = list_med_movel[1][j] - media_alt_inicio;
  }
  i = filt_i;

  // Primeira media movel
  for (j=0; j<filt_i; j++) {
      media_movel = media_movel + list_med_movel[0][j];
  }
  media_movel = media_movel / filt_i;

  // Inicialização de variáveis
  nova_altLeitura = bmp.readAltitude() - media_alt_inicio;
  cont_sub = 0; 
  cont_subidas = 0;
  cont_desc = 0;
  ult_subida = 0;
  nova_media_movel = 0;
  antiga_media_movel = 0;
  media_movel_lg = 0;
  nova_media_movel_lg = 0;

}

// the loop function runs over and over again forever
void loop() {
  // Cria uma string para ser adicionada ao cartao
  String dataString = "";
  
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
    list_med_movel[0][j] = list_med_movel[0][j+1];
  }
  list_med_movel[0][filt_i-1] = nova_altLeitura;
    // Cálculo da Média Movel
  for (j=0; j<filt_i; j++) {
    media_movel = media_movel + list_med_movel[0][j];
  }
  nova_media_movel = media_movel / filt_i;
  
    // Mudança do vetor, considerando 20 valores mais recentes
  if (i < filt_f) {
    list_med_movel[1][i] = nova_altLeitura;
    i += 1;
  }
  if (i = filt_f) {
    for (j=0; j<filt_f; j++) {
      list_med_movel[1][j] = list_med_movel[1][j+1];
    }
    list_med_movel[1][filt_f-1] = nova_altLeitura;
  
      // Cálculo da Média Movel
    for (j=0; j<filt_f; j++) {
      media_movel_lg = media_movel_lg + list_med_movel[1][j];
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
  dataString += String(nova_altLeitura);
  dataString += "\t";
  dataString += String(nova_altLeitura);
  dataString += "\t";
  dataString += String(nova_altLeitura);
  
  // Identificação de subida/descida/apogeu
  if (cont_sub > 10) {
    dataString += "\tSubindo";
    cont_subidas = 1;
    ult_subida = nova_altLeitura;
  }
  else if (cont_desc > 10) {
    dataString += "\tDescendo";
  }
  if (cont_subidas > 0 and cont_desc == 10) {
    dataString += "\tApogeu em: ";
    dataString += String(ult_subida);
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("paulo.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  media_movel = 0;
  media_movel_lg = 0;
    
}
