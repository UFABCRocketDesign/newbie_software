#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  private:
    bool estadoBeep = false;
    bool estadoBeepInterno = false;
    int buzzerPin;
    unsigned long previousMillis = 0;
    unsigned long previousMillisInterno = 0;
    unsigned long interval;
    unsigned long beepDuration;
    int beepCount = 0;
    int maxBeeps = 0;
    

  public:
    Buzzer(int pin, unsigned long intervalMillis, unsigned long beepMillis);
    
    void beep(unsigned long currentTime, bool toneMode);

    void addSystem(bool booleano);
};

#endif
