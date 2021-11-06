#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
float alt_atual = 0;
float alt_anterior = 0;
float media = 0;
float leitura = 0;
float num_leitura = 0;
float delta = 0;
float soma = 0;
float altitude[10];
int i = 0;
float soma_mm = 0;
float media_movel = 0;
float soma_mm2 = 0;
float altitude2[10];
int j = 0;
float media_movel2 = 0;
float medicao[30];
int queda = 0;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.print("Temperature(°C ) = ");
  Serial.print("\t");
  Serial.print("Pressure(Pa) = ");
  Serial.print("\t");
  Serial.print("Altitude(m) = ");
  Serial.print("Pressure at sealevel (calculated as Pa) = ");
  Serial.print("\t");
  Serial.print("Real altitude(m) = ");
  Serial.println("\t");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (i = 0; i < 97; i++) {
    alt_atual =  bmp.readAltitude();
    soma += alt_atual;
  }
  media = soma / 97;
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  alt_atual = bmp.readAltitude() - media;
  for (int i = 9; i > 0; i--) {
    altitude[i] = altitude [i - 1];
  }
  altitude[0] = alt_atual;
  soma_mm = 0;
  for (int i = 0; i < 10; i++) {
    soma_mm += altitude[i];
  }
  media_movel = soma_mm / 10;

  Serial.print(media_movel);
  Serial.print("\t");
  Serial.print(alt_atual);
  Serial.print("\t");

  for (int j = 9; j > 0; j--) {
    altitude2[j] = altitude2 [j - 1];
  }
  altitude2[0] = media_movel;
  soma_mm2 = 0;
  for (int j = 0; j < 10; j++) {
    soma_mm2 += altitude2[j];
  }
  media_movel2 = soma_mm2 / 10;
  Serial.print(media_movel2);
  Serial.print("\t");
  //Serial.print(altitude2);
  //Serial.print("\t");
  Serial.println("\t");

  for (int k = 30; k > 0; k--) {
    medicao[k] = medicao[k - 1];
  }
  medicao[0] = media_movel2;
  queda = 0;
  for (int k = 0; k < 29; k++) {
    if (medicao[k] > medicao[k + 1]) {
      queda += 1;
    }
  Serial.println(queda);
  }
  if (queda >= 25) {
    Serial.print("O foguete está caindo");
  }
}





//digitalWrite(LED_BUILTIN, HIGH);
//Serial.print(bmp.readPressure());
//Serial.print("\t");
//digitalWrite(LED_BUILTIN, LOW);

// Calculate altitude assuming 'standard' barometric
// pressure of 1013.25 millibar = 101325 Pascal
//Serial.print(bmp.readAltitude());
//Serial.print("\t");

//Serial.print(bmp.readSealevelPressure());
//Serial.print("\t");

// you can get a more precise measurement of altitude
// if you know the current sea level pressure which will
// vary with weather and such. If it is 1015 millibars
// that is equal to 101500 Pascals.
//Serial.print(bmp.readAltitude(101500));
//Serial.print("\t");
