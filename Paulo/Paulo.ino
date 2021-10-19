#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp; // Declaração da biblioteca
const int chipSelect = 53; // Declaração de CS

#define filt_i 10
#define filt_f 20

// Variáveis para detecção de apogeu
float nova_altLeitura, cont_sub, cont_subidas, cont_desc, cont_descidas, ult_subida;
float altura_inicio, media_alt_inicio;
int j, i;
float media_movel, nova_media_movel, antiga_media_movel;
float media_movel_lg, nova_media_movel_lg;
String estado;

// Filtro dos dados
float list_med_movel[2][filt_i];

// Variáveis para utilizaçao de cartao SD
String nome_arq, txt, file, arq_number;
int number, len_nome, len_number;
boolean condition;

// Variáveis para acionamento do paraquedas
const long intervalo_p_acionar = 2000;
const long intervalo_acionado = 5000;
unsigned long currentMillis;
unsigned long previousMillis_p_acionar = 0;
unsigned long previousMillis_acionado = 0;
int cont_acionar = 0;
String acionamento = "\tDesligado";

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
  nome_arq = "phhs";
  txt = ".txt";
  number = 0;
  len_nome = nome_arq.length();
  while (condition == false) { 
    file = "";
    arq_number = String(number);
    len_number = arq_number.length();
    for (int j=0; j<(8-(len_nome+len_number)); j++) {
      arq_number = "0" + arq_number;
    }
    file = nome_arq + arq_number + txt;
    if (SD.exists(file)) {
      Serial.print(file);
      Serial.println(" exists.");
      condition = false;
    } else {
      Serial.print("Creating file: ");
      Serial.println(file);
      condition = true;
    }
    number += 1;
  }
  
  // Inicia inserindo essa informação no FILE nomeado
  File dataFile = SD.open(file, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Altura\tFiltro 1\tFiltro 2\tTemperatura(oC)\tPressao(Pa)\tPressao Nivel do Mar(Pa)\tEstado(Subida/Descida)\tApogeu\tParaquedas");
    dataFile.close();
    // print to the serial port too:
    Serial.println("Altura\tFiltro 1\tFiltro 2\tTemperatura(oC)\tPressao(Pa)\tPressao Nivel do Mar(Pa)\tEstado(Subida/Descida)\tApogeu\tParaquedas");
  }
  
  // Medicao
  for (j=0; j<filt_i; j++) {
    altura_inicio = bmp.readAltitude();
    media_alt_inicio = media_alt_inicio + altura_inicio;
    delay (100);
    list_med_movel[0][j] = altura_inicio;
  }
  media_alt_inicio = media_alt_inicio / filt_i;

  for (j=0; j<filt_i; j++) {
    list_med_movel[0][j] = list_med_movel[0][j] - media_alt_inicio;
  }

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
  cont_descidas = 0;
  ult_subida = 0;
  nova_media_movel = 0;
  antiga_media_movel = 0;
  media_movel_lg = 0;
  nova_media_movel_lg = 0;
  i = 0;

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
  // Media Movel
    // Filtro 1
  for (j=0; j<9; j++) {
    list_med_movel[0][j] = list_med_movel[0][j+1];
  }
  list_med_movel[0][filt_i-1] = nova_altLeitura;
    // Cálculo da Média Movel
  for (j=0; j<filt_i; j++) {
    media_movel = media_movel + list_med_movel[0][j];
  }
  nova_media_movel = media_movel / filt_i;
  
    // Filtro 2 - realiza filtro no filtro 1
  if (i < filt_i) {
    list_med_movel[1][i] = nova_media_movel;
    i += 1;
  }
  if (i = filt_i) {
    for (j=0; j<filt_i; j++) {
      list_med_movel[1][j] = list_med_movel[1][j+1];
    }
    list_med_movel[1][filt_i-1] = nova_media_movel;
  
      // Cálculo da Média Movel
    for (j=0; j<filt_i; j++) {
      media_movel_lg = media_movel_lg + list_med_movel[1][j];
    }
    nova_media_movel_lg = media_movel_lg / filt_i;
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
  dataString += String(nova_media_movel);
  dataString += "\t";
  dataString += String(nova_media_movel_lg);
  dataString += "\t";

  // Temperatura
  dataString += String(bmp.readTemperature());
  dataString += "\t";

  // Pressão
  dataString += String(bmp.readPressure());
  dataString += "\t";

  // Pressão ao nivel do mar
  dataString += String(bmp.readSealevelPressure());
  
  // Identificação de subida/descida/apogeu
  if (cont_sub > 10) {
    estado = "\tSubindo";
    cont_subidas = 1;
    ult_subida = nova_altLeitura;
  }
  else if (cont_desc > 10) {
    estado = "\tDescendo";
  }
  dataString += estado;
  currentMillis = millis();
  if (cont_subidas > 0 && cont_desc == 1) {
    dataString += "\tApogeu em:";
    dataString += String(ult_subida);

    // Inicia processo de acionamento paraquedas
    if (cont_acionar == 0) {
      previousMillis_p_acionar = currentMillis;
      cont_acionar = 1;
    }
  }

  // Aciona paraquedas
  if (cont_acionar == 1 && (currentMillis - previousMillis_p_acionar >= intervalo_p_acionar)) {
      digitalWrite(LED_BUILTIN, HIGH);
      acionamento = "\tAcionado";
      previousMillis_acionado = currentMillis;
      cont_acionar = 2;
    }

  // Desliga o "curto" do paraquedas
  if (currentMillis - previousMillis_acionado >= intervalo_acionado) {
    digitalWrite(LED_BUILTIN, LOW);
    acionamento = "\tDesligado";
  }
  dataString += acionamento;

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(file, FILE_WRITE);

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
