#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h> // Biblioteca de altitude
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h> // Biblioteca de magnetometro

Adafruit_BMP085 bmp; // Declaração da biblioteca para altitude
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345); // Assign a unique ID to this sensor at the same time
const int chipSelect = 53; // Declaração de CS

#define filt_i 10 // Quantidade de valores Filtro 1
#define filt_f 20 // Quantidade de valores Filtro 2
#define IGN_1 36 // LED 1
#define IGN_2 61 // LED 2
#define IGN_3 46 // LED 3
#define IGN_4 55 // LED 4

// Variáveis para detecção de apogeu
float nova_altLeitura, cont_sub, cont_subidas, cont_desc, cont_descidas, ult_subida;
float altura_inicio, media_alt_inicio;
int j, i;
float media_movel, nova_media_movel, antiga_media_movel;
float media_movel_lg, nova_media_movel_lg;
String estado, str_apogeu1, str_apogeu2;
boolean cont_apogeu = true;

// Filtro dos dados
float list_med_movel[2][filt_i];

// Variáveis para utilizaçao de cartao SD
String nome_arq, txt, file, arq_number;
int number, len_nome, len_number;
boolean condition;

// Variáveis para acionamento do paraquedas
const long intervalo_p_acionar1 = 0;
const long intervalo_p_acionar2 = 7000;
const long intervalo_acionado = 5000;
unsigned long currentMillis;
unsigned long previousMillis_p_acionar = 0;
unsigned long previousMillis_p_acionar2 = 0;
unsigned long previousMillis_acionado = 0;
unsigned long previousMillis_acionado2 = 0;
unsigned long previousMillis_acionado3 = 0;
int cont_acionar1 = 0;
int cont_acionar2 = 0;
int cont_acionar3 = 0;
String acionamento1 = "\tDesligado 1";
String acionamento2 = "\tDesligado 2";
String acionamento3 = "\tDesligado 3";

// the setup function runs once when you press reset or power the board
void setup() {
  // Inicializando o led embutido no arduino
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  
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
      //Serial.print(file);
      //Serial.println(" exists.");
      condition = false;
    } else {
      Serial.print("Creating file: ");
      Serial.println(file);
      condition = true;
    }
    number += 1;
  }

  // Verifica se magnetômetro está conectado
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");
  
  // Inicia inserindo essa informação no FILE nomeado
  File dataFile = SD.open(file, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Altura\tFiltro 1\tFiltro 2\tTemperatura(oC)\tPressao(Pa)\tPressao Nivel do Mar(Pa)\tMagnetômetro (X)\tMagnetômetro (Y)\tMagnetômetro (Z)\tEstado(Subida/Descida)\tParaquedas 1\tParaquedas 2\tParaquedas 3\tApogeu");
    dataFile.close();
    // print to the serial port too:
    Serial.println("Altura\tFiltro 1\tFiltro 2\tTemperatura(oC)\tPressao(Pa)\tPressao Nivel do Mar(Pa)\tMagnetômetro (X)\tMagnetômetro (Y)\tMagnetômetro (Z)\tEstado(Subida/Descida)\tParaquedas 1\tParaquedas 2\tParaquedas 3\tApogeu");
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

  // ---------------------------------------------------------------------------------------------
  // Zerar contagem de altitude
  if (cont_sub > 0 and cont_desc == 1) {
    cont_sub = 0;
  }
  if (cont_desc > 0 and cont_sub == 1) {
    cont_desc = 0;
  }

  // ---------------------------------------------------------------------------------------------
  // Detecção de Apogeu
  nova_altLeitura = bmp.readAltitude() - media_alt_inicio;

  // Media Movel
  antiga_media_movel = nova_media_movel;
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
    for (j=0; j<(filt_i-1); j++) {
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

  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Altura
  dataString += String(nova_altLeitura);
  dataString += "\t";
  dataString += String(nova_media_movel); // Filtro 1
  dataString += "\t";
  dataString += String(nova_media_movel_lg); // Filtro 2
  dataString += "\t";

  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Temperatura
  dataString += String(bmp.readTemperature());
  dataString += "\t";

  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Pressão
  dataString += String(bmp.readPressure());
  dataString += "\t";

  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Pressão ao nivel do mar
  dataString += String(bmp.readSealevelPressure());

  // ---------------------------------------------------------------------------------------------
  // Magnetômetro
    /* Get a new sensor event */
  sensors_event_t event; 
  mag.getEvent(&event);
  // Escrevendo - Magnetômetro
    // Eixo X
  dataString += String(event.magnetic.x);
    // Eixo Y
  dataString += String(event.magnetic.y);
    // Eixo Z
  dataString += String(event.magnetic.z);
  
  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Identificação de subida/descida/apogeu
  if (cont_sub > 1) {
    estado = "\tSubindo";
    cont_subidas = 1;
    ult_subida = nova_altLeitura;
  }
  else if (cont_desc > 10) {
    estado = "\tDescendo";
  }
  dataString += estado;
  currentMillis = millis();
  if (cont_subidas > 0 && cont_desc >= 10 && cont_apogeu == true) {
    str_apogeu1 += "\tApogeu em:";
    str_apogeu2 += String(ult_subida);
    cont_apogeu = false;
    
    // Inicia processo de acionamento paraquedas
    if (cont_acionar1 == 0) {
      previousMillis_p_acionar = currentMillis;
      previousMillis_p_acionar2 = currentMillis;
      acionamento1 = "\tA Acionar";
      cont_acionar1 = 1;
      cont_acionar2 = 1;
      cont_acionar3 = 1;
    }
  }

  // ---------------------------------------------------------------------------------------------
  // Escreve e Apresenta - Paraquedas
  // Aciona primeiro paraquedas
  if (currentMillis - previousMillis_p_acionar >= intervalo_p_acionar1 && cont_acionar1 == 1) {
    digitalWrite(LED_BUILTIN, HIGH);
    acionamento1 = "\tAcionado 1";
    previousMillis_acionado = currentMillis;
    cont_acionar1 = 2;
  }
  // Desliga o "curto" do primeiro paraquedas
  if (currentMillis - previousMillis_acionado >= intervalo_acionado) {
    digitalWrite(LED_BUILTIN, LOW);
    acionamento1 = "\tDesligado 1";
  }
  // Aciona segundo paraquedas
  if (currentMillis - previousMillis_p_acionar2 >= intervalo_p_acionar2 && cont_acionar2 == 1) {
    digitalWrite(IGN_1, HIGH);
    acionamento2 = "\tAcionado 2";
    previousMillis_acionado2 = currentMillis;
    cont_acionar2 = 2;
  }
  // Desliga o "curto" do segundo paraquedas
  if (currentMillis - previousMillis_acionado2 >= intervalo_acionado) {
    digitalWrite(IGN_1, LOW);
    acionamento2 = "\tDesligado 2";
  }
  // Aciona terceiro paraquedas
  if (nova_media_movel_lg <= 10 && cont_acionar3 == 1) {
    digitalWrite(IGN_2, HIGH);
    acionamento3 = "\tAcionado 3";
    previousMillis_acionado3 = currentMillis;
    cont_acionar3 = 2;
  }
  if (currentMillis - previousMillis_acionado3 >= intervalo_acionado) {
    digitalWrite(IGN_2, LOW);
    acionamento3 = "\tDesligado 3";
  }
  // ---------------------------------------------------------------------------------------------
  
  dataString += acionamento1;
  dataString += acionamento2;
  dataString += acionamento3;
  dataString += str_apogeu1;
  dataString += str_apogeu2;

  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Informações indo para dentro do cartão SD
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

  // ---------------------------------------------------------------------------------------------
  // Reinicia variáveis das médias móveis dos filtros
  media_movel = 0;
  media_movel_lg = 0;  
}
