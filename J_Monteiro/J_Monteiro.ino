#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
const int chipSelect = 53;
//========================================================
#define n 25         // Número de medições para da média móvel
#define numReads 15 //Número de medições para a média da base
Adafruit_BMP085 bmp;
//========================================================
//-----Variáveis Globais-----

float altitude = 0;            //Altitude
float relative_average;       //Média Relativa
float vet[n] ;               //Vetor 1
float Altbase;              //Altitude no solo
float accAltbase = 0;      //Altitude inicial base acumulativa
float Maximum_height;     //Altura máxima
float previous = 0;      //Altitude anterior
float moving_average;   //Média móvel
float vet2[n];         //Vetor 2
int cont;             //Contador



void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  pinMode(LED_BUILTIN,OUTPUT); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 }
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  Serial.print("Temperature =  \t");
  Serial.print(" Pa \t");
  Serial.print("  Pressure = \t");
  Serial.print(" Pa \t");
  Serial.print("  Pressure at sealevel (calculated) = \t");
  Serial.print(" Pa \t");
  Serial.print("  Real altitude = \t");
  Serial.println(" meters \t");
 
 for (float k = 0; k < numReads; k++) {
    accAltbase = accAltbase + bmp.readAltitude();
  }
  Altbase = accAltbase / numReads;   //Média das medições do foguete na base
  Serial.print(Altbase);
  Serial.println('\t');
 
}
void loop() {

String dataString = "";

  float Height = 0;
  float current_height;

  current_height = bmp.readAltitude() - Altbase; //Transformar altitude em altura
  dataString += String(current_height);
  dataString += "\t";



  //
  //  Serial.print(bmp.readTemperature() );
  //  Serial.print('\t');
  //
  //  Serial.print( bmp.readPressure()  );
  //  Serial.print('\t');
  //  // Calculate altitude assuming 'standard' barometric
  //  // pressure of 1013.25 millibar = 101325 Pascal
  //  Serial.print( current_Altitude);
  //  Serial.print('\t');
  //
  //  Serial.print( bmp.readSealevelPressure() );
  //  Serial.print('\t');
  //
  //
  //  // vary with weather and such. If it is 1015 millibars
  //  // that is equal to 101500 Pascals.
  //
  //  Serial.print( bmp.readAltitude(101500));
  //  Serial.print('\t');

  //
  //  Serial.print(current_high);
  //  Serial.print('\t');

  //Primeira camada de filtro

  for ( int i = n - 1; i > 0 ; i --) {

    vet[i]  =  vet [i - 1];

  }
  vet[0] = current_height;

  Height = 0;

  for ( int i = 0; i < n ; i ++) {

    Height = Height + vet [i];
  }
  relative_average = Height / n;
  dataString += String(relative_average);
  dataString += "\t";
  //Segunda camada de Fitro

  for ( int i = n - 1; i > 0 ; i --) {

    vet2[i]  =  vet2 [i - 1];

  }
  vet2[0] = relative_average;

  Height = 0;

  for ( int i = 0; i < n ; i ++) {

    Height = Height + vet2[i];
  }
  moving_average = Height / n;
  dataString += String(moving_average);
  dataString += ",";
  //Apogee detection
  if ( moving_average >= previous ) {
    cont = 0;
  dataString += String(moving_average);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)           // turn the LED on (HIGH is the voltage level)
  }

  else {
    cont = cont + 1;
  dataString += ("0");     //descida
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)  // turn the LED on (LOW is the voltage level);
  }

  dataString += "\t";
  dataString += String(cont);   
  dataString += "\t";
 
  if (cont >= 50) {
  dataString += ("1");             //apogeu detectado
     }

  else {
  dataString += ("0");
  }
  dataString += "\t";
  previous =  moving_average;

  File dataFile = SD.open("JaqueMnt.txt", FILE_WRITE);

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

} 
  
