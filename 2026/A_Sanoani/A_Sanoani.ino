/**
 * Altimeter with Fall Detection
 * Sensor: BMP085 via I2C
 *
 * Pipeline per cycle:
 *   raw altitude → moving average → EMA filter → velocity → fall detection
 */

#include <Adafruit_BMP085.h>

// ─── Hardware ────────────────────────────────────────────────────────────────

Adafruit_BMP085 bmp;

// ─── Calibration ─────────────────────────────────────────────────────────────

const int CALIBRATION_SAMPLES = 100;
float baselineAltitude_m = 0.0f;

// ─── Moving Average Filter ───────────────────────────────────────────────────

const int MOVING_AVG_WINDOW = 10;
float movingAvgBuffer_m[MOVING_AVG_WINDOW];
float movingAvgRunningTotal_m = 0.0f;
int bufferWriteIndex = 0;
float movingAvgAltitude_m = 0.0f;

// ─── EMA (Exponential Moving Average) Filter ─────────────────────────────────

const float EMA_ALPHA = 0.1f;  // smaller = smoother, more lag
float smoothedAltitude_m = 0.0f;

// ─── Velocity & Fall Detection ───────────────────────────────────────────────

const float FALL_VELOCITY_THRESHOLD_MS = -2.0f;  // m/s — negative = descending
const int FALL_CONFIRM_SAMPLES = 3;              // consecutive samples to confirm fall

float rawRelativeAltitude_m = 0.0f;
float previousSmoothedAltitude_m = 0.0f;
float verticalVelocity_ms = 0.0f;
float previousTimestamp_ms = 0.0f;
int fallConfirmCounter = 0;
bool isFalling = false;

// ─── Prototypes ──────────────────────────────────────────────────────────────

void calibrateBaseline();
void updateMovingAverage(float newSample_m);
void updateEmaFilter();
void updateVelocityAndFallDetection(float currentTimestamp_ms);
void printTelemetry();

// ─────────────────────────────────────────────────────────────────────────────

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  calibrateBaseline();

  // Zero-initialise the moving average buffer
  for (int i = 0; i < MOVING_AVG_WINDOW; i++) {
    movingAvgBuffer_m[i] = 0.0f;
  }

  Serial.println(
    "Temperature(*C)\tPressure(Pa)\tAltitude(meters)\t"
    "Pressure at sealevel (calculated)(Pa)\tReal altitude(meters)");
}

void loop() {
  // 1. Raw altitude relative to ground
  rawRelativeAltitude_m = bmp.readAltitude() - baselineAltitude_m;

  // 2. Moving average filter
  updateMovingAverage(rawRelativeAltitude_m);

  // 3. EMA filter applied on top of moving average
  updateEmaFilter();

  // 4. Velocity estimation and fall detection
  float currentTimestamp_ms = static_cast<float>(millis());
  updateVelocityAndFallDetection(currentTimestamp_ms);
  previousTimestamp_ms = currentTimestamp_ms;
  previousSmoothedAltitude_m = smoothedAltitude_m;

  // 5. Serial telemetry
  printTelemetry();
}

// ─── Function Definitions ────────────────────────────────────────────────────

/**
 * Accumulates CALIBRATION_SAMPLES altitude readings and stores their
 * mean as the ground-level baseline.
 */
void calibrateBaseline() {
  float calibrationSum_m = 0.0f;
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    calibrationSum_m += bmp.readAltitude();
  }
  baselineAltitude_m = calibrationSum_m / static_cast<float>(CALIBRATION_SAMPLES);
}

/**
 * Circular-buffer moving average.
 * Subtracts the oldest sample, inserts the new one, then recalculates the mean.
 */
void updateMovingAverage(float newSample_m) {
  movingAvgRunningTotal_m -= movingAvgBuffer_m[bufferWriteIndex];
  movingAvgBuffer_m[bufferWriteIndex] = newSample_m;
  movingAvgRunningTotal_m += newSample_m;

  bufferWriteIndex++;
  if (bufferWriteIndex >= MOVING_AVG_WINDOW) {
    bufferWriteIndex = 0;
  }

  movingAvgAltitude_m = movingAvgRunningTotal_m / static_cast<float>(MOVING_AVG_WINDOW);
}

/**
 * EMA blends the moving-average output with the previous smoothed value.
 * Formula: y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
 */
void updateEmaFilter() {
  smoothedAltitude_m = (EMA_ALPHA * movingAvgAltitude_m)
                       + ((1.0f - EMA_ALPHA) * smoothedAltitude_m);
}

/**
 * Estimates vertical velocity via discrete differentiation of smoothed altitude.
 * Velocity (m/s) = Δaltitude(m) / Δtime(s).
 * millis() returns ms, so the result is multiplied by 1000.
 * Increments a confirmation counter when velocity drops below the fall threshold;
 * sets isFalling once enough consecutive samples confirm the condition.
 */
void updateVelocityAndFallDetection(float currentTimestamp_ms) {
  float deltaTime_ms = currentTimestamp_ms - previousTimestamp_ms;
  float deltaAltitude_m = smoothedAltitude_m - previousSmoothedAltitude_m;

  // Guard against division by zero on the very first cycle (deltaTime_ms == 0)
  if (deltaTime_ms > 0.0f) {
    verticalVelocity_ms = (deltaAltitude_m / deltaTime_ms) * 1000.0f;
  }

  if (verticalVelocity_ms < FALL_VELOCITY_THRESHOLD_MS) {
    fallConfirmCounter++;
    if (fallConfirmCounter > FALL_CONFIRM_SAMPLES) {
      isFalling = true;
    }
  }
}

/**
 * Emits one tab-separated telemetry line per cycle.
 * Column order matches the original: temp, pressure, raw alt,
 * moving-avg alt, smoothed alt, velocity, fall flag.
 */
void printTelemetry() {
  Serial.print(bmp.readTemperature());
  Serial.print("\t");
  Serial.print(bmp.readPressure());
  Serial.print("\t");
  Serial.print(rawRelativeAltitude_m);
  Serial.print("\t");
  Serial.print(movingAvgAltitude_m);
  Serial.print("\t");
  Serial.print(smoothedAltitude_m);
  Serial.print("\t");
  Serial.print(verticalVelocity_ms);
  Serial.print("\t");
  Serial.print(isFalling);
  Serial.print("\t");
  Serial.println();
}