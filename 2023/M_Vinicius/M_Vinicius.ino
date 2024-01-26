// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE)
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>


Adafruit_BMP085 bmp;
int i;
float altura, alt_in = 0;                       // fazer o sensor pro foguete cair, 1 --> ta caindo
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

const int chipSelect = 53;


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

   Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  Serial.print("Temperature (*C) \t");
  Serial.print("Pressure (Pa) \t");
  Serial.print("Altitude (meters) \t");
  Serial.print("Altitude sem ruido (meters) \t");
  Serial.print("Altitude s. ruido 2 (meters) \t");
  Serial.print("Detector de queda \t");



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

  filtro[index] = altura;
  index = (index + 1) % 10;
  total = 0;
  for (int i = 0; i < 10; i++) {
    total += filtro[i];
  }
  altura_semRuido = total / 10;

  filtro2[indi] =  altura_semRuido;
  indi = (indi + 1) % 10;
  acum = 0;
  for (int i = 0; i< 10; i++)
  {   
    acum += filtro2[i];
  }

  altura_sRuido2 = acum/10;        
    
        
  String dataString = "";

  Serial.print(bmp.readTemperature());
   int sensor = bmp.readTemperature();
      dataString += String(sensor);
      dataString += ",";
  Serial.print("\t");

  //
  Serial.print(bmp.readPressure());
   sensor = bmp.readPressure();
      dataString += String(sensor);
      dataString += ",";
  Serial.print("\t");

  Serial.print(altura);
  Serial.print("\t");



  Serial.print(altura_semRuido);
  
  Serial.print("\t");

  Serial.print(altura_sRuido2);
   sensor = bmp.readTemperature();
      dataString += String(sensor);
  Serial.print("\t");


  // SD CARD //

  
   File dataFile = SD.open("marquito.txt", FILE_WRITE);
        
     
     // detecção de apogeu //
  for(i = 3; i>0; i--){
    apogeu[i] = apogeu[i-1];
  }
  apogeu[0] = altura_sRuido2;
  
if (apogeu[0]<apogeu[1] && apogeu[1]<apogeu[2] && apogeu[2]<apogeu[3]){
    queda = 1;
  }
  else{
    queda = 0;
  }

    Serial.print(queda);
    Serial.print("\t");
    Serial.println();
  
  
}