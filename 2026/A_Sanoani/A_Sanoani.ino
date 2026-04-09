#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

float altitudeInicial;
float soma = 0;
int amostra = 100;
float altura_bruta = 0;
const int numReadings = 10;  // Number of readings for averaging
float readings[numReadings];
float filteredValue = 0;  // Initialize filtered value
float alpha = 0.5;        // Filter coefficient (0 < alpha < 1)
float average = 0;
float total = 0;
int readIndex = 0;

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

  Serial.print(bmp.readSealevelPressure());
  Serial.print("\t");

  Serial.println(bmp.readAltitude(101500));
}