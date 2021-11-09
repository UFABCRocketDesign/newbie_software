// Bibliotecas
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <L3G.h> // Giroscópio
#include <Adafruit_ADXL345_U.h> // Acelerômetro
#include <Adafruit_BMP085.h> // Altitude
#include <Adafruit_HMC5883_U.h> // Magnetometro

// Declaração dos bibliotecas
Adafruit_BMP085 bmp; // Altitude
Adafruit_HMC5883_Unified mag; // Magnetômetro
L3G gyro; // Giroscópio
Adafruit_ADXL345_Unified accel; // Acelerômetro
const int chipSelect = 53; // Declaração de CS

// Constantes universais
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

// SETUP DA APLICAÇÃO
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

  // Verificar se giroscópio está conectado
  Wire.begin();
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
  
  // Inicia inserindo essa informação no FILE nomeado
  File dataFile = SD.open(file, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Altura\tFiltro 1\tFiltro 2\tTemperatura(oC)\tPressao(Pa)\tPressao Nivel do Mar(Pa)\tMag (X)\tMag (Y)\tMag (Z)\tGiros (X)\tGiros (Y)\tGiros (Z)\tAcel (X)\tAcel (Y)\tAcel (Z)\tEstado(Subida/Descida)\tParaquedas 1\tParaquedas 2\tParaquedas 3\tApogeu");
    dataFile.close();
    // print to the serial port too:
    Serial.println("Altura\tFiltro 1\tFiltro 2\tTemperatura(oC)\tPressao(Pa)\tPressao Nivel do Mar(Pa)\tMag (X)\tMag (Y)\tMag (Z)\tGiros (X)\tGiros (Y)\tGiros (Z)\tAcel (X)\tAcel (Y)\tAcel (Z)\tEstado(Subida/Descida)\tParaquedas 1\tParaquedas 2\tParaquedas 3\tApogeu");
  }
  
  // Medicao
  for (j=0; j<filt_i; j++) {
    altura_inicio = bmp.readAltitude();
    media_alt_inicio = media_alt_inicio + altura_inicio;
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

  Serial.println("Final de Setup");

}

// LOOP DA SOLUÇÃO
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

  Serial.println("Final de 'Escrevendo - Altura'");
  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Temperatura
  dataString += String(bmp.readTemperature());
  dataString += "\t";

  Serial.println("Final de 'Escrevendo - Temperatura'");
  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Pressão
  dataString += String(bmp.readPressure());
  dataString += "\t";

  Serial.println("Final de 'Escrevendo - Pressão'");
  // ---------------------------------------------------------------------------------------------
  // Escrevendo - Pressão ao nivel do mar
  dataString += String(bmp.readSealevelPressure());
  dataString += "\t";

  Serial.println("Final de 'Escrevendo - nivel do mar'");
  // ---------------------------------------------------------------------------------------------
  // Magnetômetro
    /* Get a new sensor event */
  sensors_event_t event_mag; 
  mag.getEvent(&event_mag);
  // Escrevendo - Magnetômetro
    // Eixo X
  dataString += String(event_mag.magnetic.x);
  dataString += "\t";
    // Eixo Y
  dataString += String(event_mag.magnetic.y);
  dataString += "\t";
    // Eixo Z
  dataString += String(event_mag.magnetic.z);
  dataString += "\t";

  Serial.println("Final de 'Escrevendo - Magnetometro'");
  // ---------------------------------------------------------------------------------------------
  // Giroscópio
  gyro.read();
  // Escrevendo - Giroscópio
    // Eixo X
  dataString += String((int)gyro.g.x);
  dataString += "\t";
    // Eixo Y
  dataString += String((int)gyro.g.y);
  dataString += "\t";
    // Eixo Z
  dataString += String((int)gyro.g.z);
  dataString += "\t";

  Serial.println("Final de 'Escrevendo - Giroscopio'");
  // ---------------------------------------------------------------------------------------------
  // Acelerômetro
  sensors_event_t event_accel; 
  accel.getEvent(&event_accel);
  // Escrevendo - Acelerômetro
    // Eixo X
  dataString += String(event_accel.acceleration.x);
  dataString += "\t";
    // Eixo Y
  dataString += String(event_accel.acceleration.y);
  dataString += "\t";
    // Eixo Z
  dataString += String(event_accel.acceleration.z);

  Serial.println("Final de 'Escrevendo - Acelerometro'");
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
  Serial.println("Final de 'Escrevendo - Identificação de subida/descida/apogeu'");

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
  Serial.println("Final de 'Escrevendo - Paraquedas'");
  
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
