#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float altitudeInicial;
float soma = 0;
int amostra = 100;
float altura_bruta = 0;
const int numReadings = 10;   // Number of readings for averaging
float readings[numReadings];  //----filtro
float filteredValue = 0;      // Initialize filtered value
float alpha = 0.1;            // Filter coefficient (0 < alpha < 1)
float average = 0;
float total = 0;
int readIndex = 0;
const float LIMIAR_QUEDA_MS = -2.0;  // m/s (negativo = descendo)----queda
const int AMOSTRAS_CONFIRM = 3;      // amostras consecutivas p/ confirmar queda
int confirm_queda = 0;
float altura_anterior = 0;
float vel = 0;
float tempo_ant = 0;
bool queda = false;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  for (int i = 0; i < amostra; i++) {
    soma += bmp.readAltitude();
  }
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
  altitudeInicial = soma / amostra;
  Serial.println("Temperature(*C)\tPressure(Pa)\tAltitude(meters)\tPressure at sealevel (calculated)(Pa)\tReal altitude(meters)");
}


void loop() {
  altura_bruta = bmp.readAltitude() - altitudeInicial;
  total = total - readings[readIndex];

  // Read the sensor
  readings[readIndex] = altura_bruta;

  // Add the reading to the total
  total = total + readings[readIndex];

  // Advance to the next position in the array
  readIndex = readIndex + 1;

  // If we're at the end of the array, wrap around to the beginning
  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  // Calculate the average
  average = total / numReadings;

  filteredValue = (alpha * average + (1 - alpha) * filteredValue);

  float temp_agr = millis();

  vel = (filteredValue - altura_anterior) / (temp_agr - tempo_ant);

  if (vel < LIMIAR_QUEDA_MS) {
    confirm_queda = confirm_queda + 1;
    if (confirm_queda > AMOSTRAS_CONFIRM) {
      queda = true;
    }
  }

  tempo_ant = temp_agr;

  altura_anterior = filteredValue;

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  Serial.print(altura_bruta);
  Serial.print("\t");

  Serial.print(average);
  Serial.print("\t");

  Serial.print(filteredValue);
  Serial.print("\t");

  Serial.print(vel);
  Serial.print("\t");

  Serial.print(queda);
  Serial.print("\t");

  Serial.println();
}