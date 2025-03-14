#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;
#define N 3
#define L 5
#define H 4
const int chipSelect = 53;
bool h;
float med_alt = 0; 
float c[N][L];
float vFiltro[N+1];
float ordH[H];
int k=0;
float troca=0;
void setup() {
  Serial.begin(115200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  while (1);
  }
  Serial.println("card initialized.");

  
  Serial.println("Temperature\tPressure\tAltitude\tPressure\tAltitude\t");
  for(int i =0;i<10;i++){
    med_alt += bmp.readAltitude();
  }
 
  med_alt /=10; 

  }
}

void loop() {

    String dataString = "";

    troca= bmp.readTemperature();
    Serial.print(troca);
    Serial.print("\t");

    dataString += String(troca);
    dataString += "\t";

    troca = bmp.readPressure();
    Serial.print(troca);
    Serial.print("\t");

    dataString += String(troca);
    dataString += "\t";
    
    vFiltro[0] = bmp.readAltitude()- med_alt;
    
    for (int i =0 ; i<N;i++){
      c[i][k]= vFiltro[i];
      vFiltro[i+1] =0;
      for(int j =0;j<L;j++){
        vFiltro[i+1]+= c[i][j];
      }
      vFiltro[i+1]/=5;
    }
    
    k+=1;
    k%=5;
    for(int i=0; i<N+1; i++){
      Serial.print(vFiltro[i]);
      Serial.print("\t");
      dataString += String(vFiltro[i]);
      dataString += "\t";
    }

    troca = bmp.readSealevelPressure();
    Serial.print(troca);
    Serial.print("\t");

    dataString += String(troca);
    dataString += "\t";

    for(int i=H-1;i>0;i--){
       ordH[i] = ordH[i-1];
    }
    ordH[0] = vFiltro[N];
    h = true;
    for(int i=0;i<H-1;i++){
      h = h && ordH[i]<ordH[i+1];
      
    }
    
    Serial.print(h);
    Serial.print("\t"); 

    dataString += String(h);
    dataString += "\t";
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    troca = bmp.readAltitude(101500);
    Serial.print(troca);
    Serial.println("\t");

    dataString += String(troca);
    dataString += "\t";
    
    
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println(dataString);
    }
    else {
      Serial.println("error opening datalog.txt");
    }


}
