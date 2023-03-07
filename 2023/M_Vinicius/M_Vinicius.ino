// BPM085 SENSOR ( PRESSÃO / TEMPERATURA / ALTITUDE)
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
int i, w;
float altura, alt_in = 0;                       // fazer o sensor pro foguete cair, 1 --> ta caindo
float altura_semRuido = 0;
float altura_sRuido2 = 0;
float vetor[10];
float filtro[10];
float filtro2[10];
float queda;
int index = 0;
int indi = 0;
int p = 0;
float total = 0;
float acum = 0;


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

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


  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  //
  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(altura);
  Serial.print("\t");



  Serial.print(altura_semRuido);
  Serial.print("\t");

  Serial.print(altura_sRuido2);
  Serial.print("\t");

      vetor[p] = altura_sRuido2;
      p = (p + 1) % 10;
      for (w=0; w<9; w++)
      {
        if (vetor[w+1]>vetor[w])
        {
          queda = 0;
        }
        else {
          queda = 1;
        }        
      }
    

    Serial.print(queda);
    Serial.print("\t");
    


  Serial.println();
  delay(5);

}

/*for (i = 0; i<10; i++)
    {
      vetor[i] = altura_sRuido2;
      for (w=0; w<9; w++)
      {
        if (vetor[w+1]>vetor[w])
        {
          queda = 0;
        }
        else {
          queda = 1;
        }        
      }
    }*/