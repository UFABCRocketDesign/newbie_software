#include "buzzer.h"

Buzzer::Buzzer(int pin, unsigned long intervalMillis, unsigned long beepMillis) : buzzerPin(pin), interval(intervalMillis), beepDuration(beepMillis) {
  pinMode(buzzerPin, OUTPUT);
  noTone(buzzerPin);
}

void Buzzer::beep(unsigned long currentTime, bool toneMode) {
    if (estadoBeep) {
      if (estadoBeepInterno) {
        if (toneMode) {
          tone(buzzerPin, 1000);

        }
        else {
          digitalWrite(buzzerPin, LOW);
        }

      }
      else {
        if (toneMode) {
          noTone(buzzerPin);

        }
        else {
          digitalWrite(buzzerPin, HIGH);
        }


      }
      if (currentTime - previousMillisInterno > beepDuration) {
        estadoBeepInterno = !estadoBeepInterno;
        previousMillisInterno = millis();
        if (++beepCount > maxBeeps*2) {
          beepCount = 0;
          estadoBeep = !estadoBeep;
          estadoBeepInterno = false;

        }
      }
    }
    else {
      if (toneMode) {
          noTone(buzzerPin);

        }
        else {
          digitalWrite(buzzerPin, HIGH);
        }
        

    }
    if (currentTime - previousMillis > interval) {
        estadoBeep = !estadoBeep;
        previousMillis = millis();
        previousMillisInterno = previousMillis;

    }
    maxBeeps = 0;
}

void Buzzer::addSystem(bool booleano) {
  if (booleano) {
    maxBeeps++;
  }
}