#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_ADXL345_U.h>
#include <L3G.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_BMP085 bmp;
L3G gyro;

#define use_tempo 0
#define use_sd 0
#define use_bar 1
#define use_gyro 0
#define use_mag 0
#define use_accel 0
#define print_serial 0

#define use_alt (use_bar && 0)
#define use_pressao (use_bar && 0)
#define use_temp (use_bar && 1)
#define use_apogeu (use_alt && 0)

#define use_gyro_x (use_gyro && 1)
#define use_gyro_y (use_gyro && 1)
#define use_gyro_z (use_gyro && 1)

#define use_mag_x (use_mag && 1)
#define use_mag_y (use_mag && 1)
#define use_mag_z (use_mag && 1)

#define use_accel_x (use_accel && 1)
#define use_accel_y (use_accel && 1)
#define use_accel_z (use_accel && 1)

#define l 20 // tamanho
#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/
#define ledPin LED_BUILTIN

#if (use_alt)
float velhaAlt = 0.0;
float altura_referencia = 0.0;
float lista[l];
float lista2[l];
#endif

#if use_apogeu
int encontra_apogeu = 0;
bool apogeu_detectado = false;
bool laco_led_2 = false;      // variavel para entrar no laço liga led 2
bool laco_led_3 = false;      // variavel para entrar no laço liga led 3 (built in)
bool ledState1 = LOW;    // ledState used to set the LED
bool ledState2 = LOW;
bool ledState3 = LOW;
#define h_paraquedas_2 10  // altura para acionar 2º paraquedas (led2)
#define interv_desliga_led 7000      // interval at which to blink (milliseconds)
#define interv_liga_led2 4000
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long liga_led2 = 0;        // quando o led tem que acender
unsigned long desliga_led1 = 0;
unsigned long desliga_led2 = 0; // quando o led tem que desligar após o apogeu
unsigned long desliga_led3 = 0;
#endif

#define chipSelect 53
String nomeArquivo = "";

//////// Função: filtro altura ///////////

#if use_alt
float filtro_altura(float entrada, int i)
{
  float media_mov = 0.0;
  if (i == 1) {
    for (int k = 0; k < (l - 1); k++) {
      lista[k] = lista[k + 1];
    }
    lista[l - 1] = entrada;
    for (int j = 0; j < l; j++) {
      media_mov = media_mov + lista[j];
    }
    media_mov = media_mov / l;
  }

  else if (i == 2) {
    for (int k = 0; k < (l - 1); k++) {
      lista2[k] = lista2[k + 1];
    }
    lista2[l - 1] = entrada;
    for (int j = 0; j < l; j++) {
      media_mov = media_mov + lista2[j];
    }
    media_mov = media_mov / l;
  }
  return media_mov;
}
#endif

////////////////////////////////////////

void setup() {
  String cabecalho = "";
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  // TESTANDO SE OS SENSORES/SD ESTÃO FUNCIONANDO

#if use_bar
  if (!bmp.begin()) {
#if print_serial
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
#endif // print_serial
#endif // bar

#if use_mag
  /* Initialise the mag sensor */
  if (!mag.begin())
  {
#if print_serial
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
#endif
  }
#endif  //mag

#if use_accel
  /* Initialise the sensor */
  if (!accel.begin())
  {
#if print_serial
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
#endif
  }
#endif   //accel

#if use_gyro
  /* gyro */
  if (!gyro.init())
  {
#if print_serial
    Serial.println("Failed to autodetect gyro type!");
#endif
  }
  gyro.enableDefault();
#endif   // gyro

#if use_sd
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
#if print_serial
    Serial.println("Card failed, or not present");
#endif
  }
#if print_serial
  Serial.println("card initialized.");
#endif // print_serial
#endif // use_sd

#if use_accel
  // ACCELEROMETER RANGE

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // accel.setRange(ADXL345_RANGE_8_G);
  // accel.setRange(ADXL345_RANGE_4_G);
  // accel.setRange(ADXL345_RANGE_2_G);
#endif

#if (use_alt)
  // MÉDIA DE ALTITUDE

  for (int i = 0; i < 20; i++) {
    altura_referencia =  altura_referencia + bmp.readAltitude();
  }
  altura_referencia = altura_referencia / 20;
#endif

  // CABEÇALHO

#if use_tempo
  cabecalho += "Tempo [s]\t";
#endif

#if (use_alt)
  cabecalho += "Altura [m]\tFiltro1 (h)\tFiltro2 (h)\t";
#endif

#if (use_temp)
  cabecalho += "Temperatura [*C]\t";
#endif

#if (use_pressao)
  cabecalho += "Pressao [Pa]\t";   //Pressao no nivel do mar [Pa]";
#endif

#if use_mag_x
  cabecalho += "Mag eixo X [uT]\t";
#endif
#if use_mag_y
  cabecalho += "Mag eixo Y [uT]\t";
#endif
#if use_mag_z
  cabecalho += "Mag eixo Z [uT]\t";
#endif

#if use_accel_x
  cabecalho += "Accel eixo X [m/s^2]\t";
#endif
#if use_accel_y
  cabecalho += "Accel eixo Y [m/s^2]\t";
#endif
#if use_accel_z
  cabecalho += "Accel eixo Z [m/s^2]\t";
#endif

#if use_gyro_x
  cabecalho += "Gyro eixo X\t";
#endif
#if use_gyro_y
  cabecalho += "Gyro eixo Y\t";
#endif
#if use_gyro_z
  cabecalho += "Gyro eixo Z\t";
#endif

#if (use_alt)
  cabecalho += "Apogeu\t";
#endif

  // CRIANDO UM NOVO ARQUIVO PARA SALVAR NO SD

#if use_sd
  String nome = "gabi";
  int tamNome = nome.length();
  int num = 0;
  String txt = ".txt";
  bool tmp = false;
  while (tmp == false) {
    String zeros = "";
    int tamNum = String(num).length();
    int tamTotal = tamNome + tamNum;
    for (int j = 0; j < (8 - tamTotal); j++) {
      zeros = zeros + "0";
    }
    nomeArquivo = nome + zeros + String(num) + txt;
    if (SD.exists(nomeArquivo)) {
      num = num + 1;
      tmp = false;
    }
    else {
      File dataFile = SD.open(nomeArquivo, FILE_WRITE);
      dataFile.println(cabecalho);
      dataFile.close();
      tmp = true;
    }
  }
#if print_serial
  Serial.println("file: " + nomeArquivo);
#endif // print_serial
#endif // use_sd

#if print_serial
  Serial.println(cabecalho);
#endif
}

void loop() {
  unsigned long t_atual = millis();
  float t_atual_segundos = t_atual / 1000.0;
  // make a string for assembling the data to log:
  String dataString = "";

  //// LEITURA DE SENSORES

#if use_alt
  float h = 0.0;
  // Calculate altitude assuming 'standard' barometric pressure of 1013.25 millibar = 101325 Pascal
  float novaAlt = bmp.readAltitude();
  h = novaAlt - altura_referencia;  //altura
#endif

#if use_temp
  //temperatura
  float temp =  bmp.readTemperature();
#endif

#if use_pressao
  //pressão
  float pressao = bmp.readPressure();
  //pressão no nível do mar
  //float pressaoNivelMar = bmp.readSealevelPressure();
#endif

#if (use_mag || use_accel)
  /* Get a new sensor event */
  sensors_event_t event;
#endif

#if use_mag
  // Magnetometro
  mag.getEvent(&event);
#if use_mag_x
  float mag_X;
  mag_X = event.magnetic.x;
#endif
#if use_mag_y
  float mag_Y;
  mag_Y = event.magnetic.y;
#endif
#if use_mag_z
  float mag_Z;
  mag_Z = event.magnetic.z;
#endif

#endif  // use_mag

#if use_accel
  // Acelerometro
  accel.getEvent(&event);
#if use_accel_x
  float accel_X;
  accel_X = event.acceleration.x;
#endif
#if use_accel_y
  float accel_Y;
  accel_Y = event.acceleration.y;
#endif
#if use_accel_z
  float accel_Z;
  accel_Z = event.acceleration.z;
#endif

#endif   // use_accel

#if use_gyro
  // Giroscopio
  gyro.read();
#if use_gyro_x
  float gyro_X;
  gyro_X = (int)gyro.g.x;
#endif
#if use_gyro_y
  float gyro_Y;
  gyro_Y = (int)gyro.g.y;
#endif
#if use_gyro_z
  float gyro_Z;
  gyro_Z = (int)gyro.g.z;
#endif

#endif  // use_gyro

  //// FILTROS DE ALTURA

#if use_alt
  // filtro 1
  float media_mov = filtro_altura(h, 1);

  // filtro 2
  float media_mov2 = filtro_altura(media_mov, 2);
#endif

  //// REUNINDO OS DADOS EM UMA STRING

#if use_tempo
  dataString += String(t_atual_segundos);
  dataString += "\t";
#endif
#if use_alt
  // dataString += String(novaAlt);
  // dataString += "\t";
  dataString += String(h);
  dataString += "\t";
  dataString += String(media_mov);  // media_mov é o filtro 1 de altura
  dataString += "\t";
  dataString += String(media_mov2);  // media_mov2 é o filtro 2 de altura
  dataString += "\t";
#endif
#if use_temp
  dataString += String(temp);
  dataString += "\t";
#endif
#if use_pressao
  dataString += String(pressao);
  dataString += "\t";
  //dataString += String(pressaoNivelMar);
  //dataString += "\t";
#endif

#if use_mag_x
  dataString += String(mag_X);
  dataString += "\t";
#endif
#if use_mag_y
  dataString += String(mag_Y);
  dataString += "\t";
#endif
#if use_mag_z
  dataString += String(mag_Z);
  dataString += "\t";
#endif
#if use_accel_x
  dataString += String(accel_X);
  dataString += "\t";
#endif
#if use_accel_y
  dataString += String(accel_Y);
  dataString += "\t";
#endif
#if use_accel_z
  dataString += String(accel_Z);
  dataString += "\t";
#endif
#if use_gyro_x
  dataString += String(gyro_X);
  dataString += "\t";
#endif
#if use_gyro_y
  dataString += String(gyro_Y);
  dataString += "\t";
#endif
#if use_gyro_z
  dataString += String(gyro_Z);
  dataString += "\t";
#endif

  // Encontrando o apogeu

#if use_apogeu
  if (media_mov2 < velhaAlt) {
    encontra_apogeu += 1;
  }
  else {
    encontra_apogeu = 0;
  }

  if (encontra_apogeu <= 5) {
    dataString += "Apogeu foi detectado! Descendo.";
    dataString += "\t";
    if (apogeu_detectado == false) {
      // LED 1
      ledState1 = HIGH;
      desliga_led1 = t_atual + interv_desliga_led;
      liga_led2 = t_atual + interv_liga_led2;
      apogeu_detectado = true;
      laco_led_2 = true;
      laco_led_3 = true;
    }
  }
  // LED 2
  if (t_atual >= liga_led2 && laco_led_2 == true) {
    ledState2 = HIGH;
    dataString += "Led 2!";
    dataString += "\t";
    desliga_led2 = t_atual + interv_desliga_led;
    laco_led_2 = false;
  }
  // LED 3
  if (media_mov2 <= h_paraquedas_2 && laco_led_3 == true) {
    ledState3 = HIGH;
    dataString += "Led 3!";
    dataString += "\t";
    desliga_led3 = t_atual + interv_desliga_led;
    laco_led_3 = false;
  }

  if (t_atual >= desliga_led1) {
    ledState1 = LOW;
  }
  if (t_atual >= desliga_led2) {
    ledState2 = LOW;
  }
  if (t_atual >= desliga_led3) {
    ledState3 = LOW;
  }

  digitalWrite(IGN_1, ledState1);
  digitalWrite(IGN_2, ledState2);
  digitalWrite(ledPin, ledState3);
#endif // use_apogeu

  // Mostrando dataString no Serial
#if print_serial
  Serial.println(dataString);
#endif

#if use_sd
  // abrir o arquivo do SD e armazenar dados
  File dataFile = SD.open(nomeArquivo, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
#endif

#if use_alt
  // Reiniciando variável de velha altitude
  velhaAlt = media_mov2;
#endif
}
