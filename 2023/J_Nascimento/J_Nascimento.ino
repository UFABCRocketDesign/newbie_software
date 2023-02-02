
#include <Adafruit_BMP085.h>



Adafruit_BMP085 bmp;
float alt_sem_ruido = 0;
float alt_average = 0;
float alts[10];
float media = 0;
float sum = 0;

void setup() {

    
    Serial.begin(115200);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    }
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);


    // calculating average altitude value's
    for(int i = 0; i < 50; i++){
      alt_average = alt_average + bmp.readAltitude();
    }
    alt_average = alt_average/50;

    // showing the header
    Serial.println("Temperature( Cº)\tPressure (Pa)\tAltitude_sem_ruido");
    
}

//----------------------------------------------------------------------------------------------------
void loop() {
    int i = 0;
    Serial.print(bmp.readTemperature());

    Serial.print("\t");
    Serial.print(bmp.readPressure());
    
    Serial.print("\t");
    for(int j = 0; j < 10; j++){
      alts[j] = bmp.readAltitude();
      alts[j] = alts[j+1];
      sum = sum + alts[j+1];
      media = sum/j;    
    }
    alt_sem_ruido = (bmp.readAltitude()) - media;
    Serial.print(alt_sem_ruido);

    Serial.println();
}
