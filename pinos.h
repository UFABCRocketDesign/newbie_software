#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif // ARDUINO_AVR_MEGA2560

#define IGN_1 36	/*act1*/
#define IGN_2 61	/*act2*/
#define IGN_3 46	/*act3*/
#define IGN_4 55	/*act4*/

#define HEAL_1 68	/*health 1*/
#define HEAL_2 62	/*health 2*/
#define HEAL_3 56	/*health 3*/
#define HEAL_4 58	/*health 4*/

#define BUZZ_PIN A0
#define BUZZ_CMD LOW

HardwareSerial &LoRa(Serial3);
HardwareSerial &GPS(Serial1);


#define M0_LORA_PIN 12 // Pinos adicionais do LoRa
#define M1_LORA_PIN 11 // Pinos adicionais do LoRa
#define AUX_LORA_PIN A8 // Pinos adicionais do LoRa

#define USANDO_SD 1

#if USANDO_SD
// Codigo pro cartao SD
#endif
