#include <Adafruit_BMP085.h>
#define led 13 
#define pmt 20
Adafruit_BMP085 bmp;
float z, const_chao, alt_ref, sinal_filtrado;
float vetor[pmt];

void setup() {
 pinMode(led, OUTPUT);
 Serial.begin(115200);
 if (!bmp.begin()) {
 Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

 //media para referenciar a altura 
  cal_chao();
  
  Serial.print("Temp (*C)\t");
  Serial.print("Pres (Pa)\t");
  Serial.print("Alt (m)\t");
  Serial.println();

}

void loop() {

// Serial.print(bmp.readTemperature());
// Serial.print("\t");
// Serial.print(bmp.readPressure());
// Serial.print("\t");

//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(500);
//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(500);
//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(500);
//  digitalWrite(led, HIGH);
//  delay(2000);
//  digitalWrite(led, LOW);
//  delay(2000);

  media_movel();
 alt_ref = bmp.readAltitude() - const_chao;
 Serial.print(alt_ref);
 Serial.print("\t");
 Serial.println();
 Serial.print(sinal_filtrado);
 Serial.print("\t");
 Serial.println();
 
}
//-----------------------------------------------------------------------------


void cal_chao() {
  z = 0;
 for(int x = 0; x < 100; x++){
   z = bmp.readAltitude() + z;
 }
 const_chao = z/100;
}


//----------------------------------------------------------------------------------

void media_movel() {

 for(int x = pmt; x > 0; x--){
   vetor[x]= bmp.readAltitude() - const_chao;
 }

 int k = 0;
 for(int x = 0; x < pmt; x++){
   k = vetor[x] + k;
 }
 
sinal_filtrado = k/pmt;

}
