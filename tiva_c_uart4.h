#ifndef TIVA_C_UART
#define TIVA_C_UART

#include <stdint.h>
#include <stdbool.h>


#define BAUD_RATE																						115200

#define LENGTH_OF_DATA																			0x05
#define BUFFER_OF_DATA																			(4 * LENGTH_OF_DATA)



void UART4Initialize(void);
void UART4PortKInitialize(void);
void UART4PortKGPIOInitialize(void);
void UART4SendData(uint8_t *data_buffer, uint8_t length);
void UART4InitializeInterrupt(void);

#endif
