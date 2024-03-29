// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE)
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
Adafruit_BMP085 bmp;

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif  // ARDUINO_AVR_MEGA2560

#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/

const int paraquedasPin = IGN_1;
int paraquedas = LOW;
unsigned long previousMillis = 0;
const long interval = 3000;

int i;
float altura, alt_in = 0;  // fazer o sensor pro foguete cair, 1 --> ta caindo
float altura_semRuido = 0;
float altura_sRuido2 = 0;
float vetor[10];
float filtro[10];
float filtro2[10];
float queda;
int index = 0;
int indi = 0;
float total = 0;
float acum = 0;
float apogeu[4];
int ap = 0;
int indicador = 0;

String marcos;
String marcs = "marcs";
String nome_do_arquivo;

const int chipSelect = 53;


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(paraquedasPin, OUTPUT);

  Serial.begin(115200);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");

  // CRIAR UM NOVO ARQUIVO DE TEXTO CADA VEZ QUE O CARTÃO SD É INSERIDO //

  do {
    String qnt_zero;
    for (i = String(indicador).length() + String(marcs).length(); i < 8; i++) {
      qnt_zero += "0";
    }

    nome_do_arquivo = marcs + qnt_zero + String(indicador) + ".txt";

    indicador++;


  } while (SD.exists(nome_do_arquivo));

  Serial.println(nome_do_arquivo);




  // ADICIONAR O CABEÇALHO //

  String cabString = "";

  cabString += ("Temperature (*C)");
  cabString += "\t";

  cabString += ("Pressure (Pa)");
  cabString += "\t";

  cabString += ("Altitude (meters)");
  cabString += "\t";

  cabString += ("Altitude sem ruido (meters)");
  cabString += "\t";

  cabString += ("Altitude s. ruido 2 (meters)");
  cabString += "\t";

  cabString += ("Detector de queda");
  cabString += "\t";

  cabString += ("Estado paraquedas");
  cabString += "\t";

  File cabFile = SD.open(nome_do_arquivo, FILE_WRITE);

  // if the file is available, write to it:
  if (cabFile) {
    cabFile.println(cabString);
    cabFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening" + nome_do_arquivo);
  }
  Serial.println(cabString);



  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }


  for (i = 0; i < 5; i++) {
    alt_in = alt_in + bmp.readAltitude();
  }

  alt_in = alt_in / 5;
}


void loop() {

  altura = bmp.readAltitude() - alt_in;

  // filtro 1 //

  filtro[index] = altura;
  index = (index + 1) % 10;
  total = 0;
  for (int i = 0; i < 10; i++) {
    total += filtro[i];
  }
  altura_semRuido = total / 10;

  // filtro 2 //

  filtro2[indi] = altura_semRuido;
  indi = (indi + 1) % 10;
  acum = 0;
  for (int i = 0; i < 10; i++) {
    acum += filtro2[i];
  }

  altura_sRuido2 = acum / 10;

  // Criação do dataString para armazenar as variaveis //
  String dataString = "";

  dataString += bmp.readTemperature();
  dataString += "\t";

  dataString += bmp.readPressure();
  dataString += "\t";

  dataString += String(altura);
  dataString += "\t";

  dataString += String(altura_semRuido);
  dataString += "\t";

  dataString += String(altura_sRuido2);
  dataString += "\t";


  // DETECTAR APOGEU //

  for (i = 3; i > 0; i--) {
    apogeu[i] = apogeu[i - 1];
  }
  apogeu[0] = altura_sRuido2;

  if (apogeu[0] < apogeu[1] && apogeu[1] < apogeu[2] && apogeu[2] < apogeu[3]) {
    queda = 1;
  } else {
    queda = 0;
  }

  dataString += String(queda);
  dataString += "\t";



  // LIBERAR O PRIMEIRO PARAQUEDAS //
   unsigned long currentMillis = millis();

  if ( apogeu[0] > apogeu[1] && apogeu[1] > apogeu[2] && queda == 0) {
     
    if (previousMillis == 0 && paraquedas == LOW) {
      paraquedas = HIGH; // ligado
    } else if (previousMillis != 0) {
      paraquedas = LOW;  //desligado
    }
    previousMillis = currentMillis;
    
    digitalWrite(paraquedasPin, paraquedas);
  }

  dataString += String(paraquedas);
  dataString += "\t";



  // SD CARD //

  File dataFile = SD.open(nome_do_arquivo, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:

  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening" + nome_do_arquivo);
  }
  Serial.println(dataString);
}