#ifndef TIVA_C_LED
#define TIVA_C_LED

#include <stdint.h>
#include <stdbool.h>

#define LED1																1
#define LED2																0
#define LED3																4
#define LED4																0
#define NUM_OF_LEDS													4


void LEDInitialize(void);
void LEDPortInitialize(void);
void TurnOnLED(uint8_t leds);
void TurnOffLED(uint8_t leds);

#endif
