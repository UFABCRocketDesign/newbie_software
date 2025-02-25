    #include <Adafruit_BMP085.h>

    Adafruit_BMP085 bmp;
    float altitudeTarada = 0;  
    float suavizador = 0.01;

    void setup() {
      Serial.begin(115200);
      if (!bmp.begin()) {
        Serial.println("Nao foi possivel encontrar o bmp085, verificar as conexoes!");
        while (1) {}
      }  

      for (int i = 0; i < 10; i++) { 
        altitudeTarada += bmp.readAltitude();

      }
      altitudeTarada /= 10; 
      float altitudeNova = bmp.readAltitude();

      Serial.println("Temperatura \t Pressao \t Altitude \t Nivel do mar \t Altitude Relativa");
    }

    void loop() {
        
      altitudeNova = ((altitudeNova*(1-suavizador))+(bmp.readAltitude()*suavizador));
        
        Serial.print(bmp.readTemperature());
        Serial.print("\t");

        Serial.print(bmp.readPressure());
        Serial.print("\t");

        Serial.print(bmp.readAltitude());
        Serial.print("\t");

        Serial.print(bmp.readSealevelPressure());
        Serial.print("\t");

        Serial.print(altitudeNova);
        Serial.println();

    }
