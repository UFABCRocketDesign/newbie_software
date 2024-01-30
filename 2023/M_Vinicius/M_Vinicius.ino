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
int indicador =0;

String marcos, qnt_zero; 
String marcs = "marcs";

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

    // CRIAR UM NOVO ARQUIVO DE TEXTO CADA VEZ QUE O CARTÃO SD É INSERIDO //

        do{
          for (i = String(indicador).length() + String(marcs).length() ; i<8; i++)
          {
            qnt_zero += "0";
          }

          marcos = marcs + qnt_zero + String(indicador) + ".txt";

          indicador++;
                

        } while(SD.exists(marcos));

        Serial.println(marcos);
         



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
    
    File cabFile = SD.open(marcos, FILE_WRITE);

                      // if the file is available, write to it:
          if (cabFile) {
              cabFile.println(cabString);
              cabFile.close();
                      // print to the serial port too:
          }
                      // if the file isn't open, pop up an error:
          else {
              Serial.println("error opening" + marcos);
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

  filtro2[indi] =  altura_semRuido;
  indi = (indi + 1) % 10;
  acum = 0;
  for (int i = 0; i< 10; i++)
  {   
    acum += filtro2[i];
  }

  altura_sRuido2 = acum/10;        
    
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

            dataString += String(queda);
            dataString += "\t";



               // SD CARD //
        
        File dataFile = SD.open(marcos, FILE_WRITE);

                      // if the file is available, write to it:
          if (dataFile) {
              dataFile.println(dataString);
              dataFile.close();
                      // print to the serial port too:
            
          }
                      // if the file isn't open, pop up an error:
          else {
              Serial.println("error opening" + marcos);
          }
        Serial.println(dataString);
  
  
}