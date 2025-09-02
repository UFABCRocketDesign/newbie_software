// Sempre verificar antes de compilar
// Foguete não tem delay
// Serial println pula a linha
// Sempre definir os valores para facilitar manutençao futura o código


// Bibliotecas aqui
#include <Adafruit_BMP085.h>
#define TAM_FILTRO_ALTURA 15

Adafruit_BMP085 bmp;

float alturazero;
float VetorFiltroAltura[TAM_FILTRO_ALTURA];
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  float val_alts=0.0;

  for (int alt = 0;alt < 15; alt++) {
  val_alts = val_alts + bmp.readAltitude();
  } 

  alturazero = val_alts/15;

  Serial.print("Temperature (*C)  ");
  Serial.print("\t");
  Serial.print("Pressure (Pa)  ");
  Serial.print("\t");
  Serial.print("Altitude (m)  ");
  Serial.print("\t");
  Serial.print("Pressure at sealevel (calculated in Pa) = ");
  Serial.print("\t");
  Serial.print("Real altitude (m)  ");
  Serial.print("\t");
  Serial.println("Altitude zerada (m)  ");
  Serial.print("\t");
  Serial.println("Altitude filtrada (m)  ");
}

void loop() {

    float altitude = bmp.readAltitude();
    float altura_bruta = altitude- alturazero ; // tem ruído, necessário um fitro

    for(int i = TAM_FILTRO_ALTURA-1; i>0; i--) {
      VetorFiltroAltura[i] = VetorFiltroAltura[i-1]; 
    }

    VetorFiltroAltura[0] = altura_bruta;

    float SomaVetorAltura = 0.0;
    for(int i=0; i<TAM_FILTRO_ALTURA; i++){
        SomaVetorAltura += VetorFiltroAltura[i];
    }
    float AlturaFiltrada = SomaVetorAltura / TAM_FILTRO_ALTURA;
  
  //fazer 5 filtros e os otimizar
    Serial.print(bmp.readTemperature());
    Serial.print("\t");
    Serial.print(bmp.readPressure());
    Serial.print("\t");
    Serial.print(altitude);
    Serial.print("\t");
    Serial.print(bmp.readSealevelPressure());
    Serial.print("\t");
    Serial.print(bmp.readAltitude(101500));
    Serial.print("\t");
    Serial.print(altura_bruta);
    Serial.print("\t");
    Serial.print(AlturaFiltrada);
    Serial.println();
}