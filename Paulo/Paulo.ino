#include <Adafruit_BMP085.h>

/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

Adafruit_BMP085 bmp; // Declaração da biblioteca
float altitudeLeitura, nova_altLeitura, cont_sub, cont_subidas, cont_desc, ult_subida;
float altura_inicio, media_alt_inicio, i;

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

  // Medicao
  for (i=0; i<10; i++) {
    altura_inicio = bmp.readAltitude();
    media_alt_inicio = media_alt_inicio + altura_inicio;
    delay (100);
  }
  media_alt_inicio = media_alt_inicio / i;

  nova_altLeitura = bmp.readAltitude() - media_alt_inicio;
  ult_subida = 0;
  cont_subidas = 0;
  cont_sub = 0;
  cont_desc = 0;
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
    if (nova_altLeitura > altitudeLeitura) {
      cont_sub += 1;
    }
    else if (nova_altLeitura < altitudeLeitura) {
      cont_desc += 1;
    }

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
    
    // BMP085 - TESTE
      // Criação de Colunas - 2º Linha:
        // Temperatura
    /*
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
        // Pressão
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    */
        // Altitude
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print(nova_altLeitura);
    Serial.println(" m");
    /*
    Serial.print("\t");
        // Pressão nivel do mar
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
        // Altitude Real
    // you can get a more precise measurement of altitude
    // if you know the current sea level pressure which will
    // vary with weather and such. If it is 1015 millibars
    // that is equal to 101500 Pascals.
    Serial.println(bmp.readAltitude(101500));
    */
       
}
