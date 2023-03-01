#include <Adafruit_BMP085.h>

/*************************************************** 
  This is an example for the BMP085 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391

  These pressure and temperature sensors use I2C to communicate, 2 pins
  are required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

Adafruit_BMP085 bmp;
float altitude, altitude_inicial=0, altura, soma_altura=0, sem_ruido1, sem_ruido2, v[10], v2[10],v3[10],soma_altura_queda=0;
int i;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  pinMode(13, OUTPUT);
  Serial.print("Temperature (*C)\t Pressure (Pa)\t Altitude (m)\t Altura(m)\t Altura sem ruído 1(m)\t Altura sem ruído 2(m)\t Queda\t ");
  for(i=0;i<5;i++){
    altitude_inicial += bmp.readAltitude();
  }
  altitude_inicial = altitude_inicial/5;

}
  
void loop() {
    Serial.print(bmp.readTemperature());
    Serial.print("\t");

  
    Serial.print(bmp.readPressure());
    Serial.print("\t");
   
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    //Serial.print("Altitude = ");
    altitude = bmp.readAltitude();
    Serial.print(altitude);
    Serial.print("\t");
  


    altura = altitude - altitude_inicial;
    Serial.print(altura);
    Serial.print("\t");

    soma_altura=0;
    //altura sem ruido 1
    for(i=9;i>0;i--){
      v[i] = v[i-1]; 
      soma_altura += v[i];
    }
    v[0] = altura;
    soma_altura += v[0];

    sem_ruido1 = soma_altura/10;
    //sem_ruido = altura - soma_altura;
    Serial.print(sem_ruido1);
    Serial.print("\t");


    //altura sem ruido 2
    soma_altura=0;
    for(i=9;i>0;i--){
      v2[i] = v2[i-1]; 
      soma_altura += v2[i];
    }
    v2[0] = sem_ruido1;
    soma_altura += v2[0];

    sem_ruido2 = soma_altura/9;
    //sem_ruido = altura - soma_altura;
    Serial.print(sem_ruido2);
    Serial.print("\t");

    //detecção de queda
    soma_altura_queda=0;
    for(i=9;i>0;i--){
      if (v3[i]<v3[i-1]){
        soma_altura_queda += 1;
      }
      v3[i] = v3[i-1]; 
    }
    v3[0] = sem_ruido2;
    if (v3[1]<v3[0]){
      soma_altura_queda += 1;      
    }
    Serial.print(soma_altura_queda);
    Serial.print("\t");  
    
    if (soma_altura_queda>=10){
      Serial.print(0);
      Serial.print("\t");
    }
    else{
      Serial.print(1);
      Serial.print("\t");      
    }



    //Serial.print("Pressure at sealevel (calculated) = ");
    //Serial.print(bmp.readSealevelPressure());
    //Serial.print("\t");


  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    //Serial.print("Real altitude = ");
    //Serial.print(bmp.readAltitude(101500));
    //Serial.print("\t");

    
    Serial.println();
    delay(40);
    
}
