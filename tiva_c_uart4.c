#include "tiva_c_uart4.h"
#include "driverlib/sysctl.h"						// System control
#include "inc/hw_ints.h"								// HW Interrupt Control
#include "inc/hw_memmap.h"							// HW Memory Map control
#include "driverlib/gpio.h"							// GPIO control
#include "driverlib/uart.h"							// UART control
#include "driverlib/pin_map.h"					// Pin Mapping for UART4
#include "driverlib/interrupt.h"				// Interrupt control
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "stream_buffer.h"              // ARM.FreeRTOS::RTOS:Stream Buffer



extern uint32_t SystemCoreClock;
extern void UART4_Handler(void);

void InitializeStreamBuffer(void);																			//Private function of this file


StreamBufferHandle_t uart_stream_buffer = NULL;


void InitializeStreamBuffer(void)
{
	uart_stream_buffer = xStreamBufferCreate(BUFFER_OF_DATA, LENGTH_OF_DATA);							//1st Arg - Size of the buffer, 2nd Arg - trigger Level
	
	if(uart_stream_buffer == NULL)
	{
		//Take any action
	}
}




void UART4PortKInitialize(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);													//Enable Clock to PORTK Peripheral
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));										//Wait till PORTK is ready
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);													//Enable Clock to UART4 Peripheral
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART4));										//Wait till UART4 is ready
}



void UART4PortKGPIOInitialize(void)
{
	GPIOPinConfigure(GPIO_PK0_U4RX);																			//Configure PORTK PIN0 as UART4 Rx
	GPIOPinConfigure(GPIO_PK1_U4TX);																			//Configure PORTK PIN1 as UART4 Tx
	GPIOPinTypeUART(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1);						//Set the PORTK PIN0 and PIN1 as UART Type, GPIO_PIN_0 -> 0x01, GPIO_PIN_1 -> 0x02
}



void UART4Initialize(void)
{
	UARTDisable(UART4_BASE);																							//Disable the UART4 Module to write various settings as set by the below function
	
	/* This function sets up the desired setting for UART Module */
	UARTConfigSetExpClk(UART4_BASE, SystemCoreClock, BAUD_RATE, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	
	UARTFIFODisable(UART4_BASE);																					//Disabling the FIFO
	
	InitializeStreamBuffer();																							//Initialize the stream buffer while initializing UART4
}



void UART4InitializeInterrupt(void)
{
	UARTIntRegister(UART4_BASE, &UART4_Handler);													//Register the UART4_Handler like this so as program should know what to call when interrupt occurs
	UARTIntEnable(UART4_BASE, UART_INT_RX | UART_INT_TX);																//This is the interrupt flag for Receive, you can select multiple by OR operation
	
	/* Setting up interrupt priority */
	IntPrioritySet(INT_UART4, 0xA0);							//configMAX_SYSCALL_INTERRUPT_PRIORITY = 0xA0;
}



void UART4SendData(uint8_t *data_buffer, uint8_t length)
{
	while(length--)
		UARTCharPut(UART4_BASE, *data_buffer++);												//Send complete data over UART4
}



/* Will have to change the ISR as we will incorporate FreeRTOS Stream Buffer APIs */

void UART4_Handler(void)
{
	uint8_t data=0;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	uint32_t interrupt_type = UARTIntStatus(UART4_BASE, true);									//Get the interrupt flag, to know which interrupt has occurred
	
	UARTIntClear(UART4_BASE, interrupt_type);																		//Clear the interrupt - acknowledgement
	
	while(UARTCharsAvail(UART4_BASE))																						//While there are bytes available
	{
		//UARTCharPut(UART4_BASE, UARTCharGetNonBlocking(UART4_BASE));
		
		data = UARTCharGetNonBlocking(UART4_BASE);
		
		xStreamBufferSendFromISR(uart_stream_buffer, (void *)&data, 1, &xHigherPriorityTaskWoken);
	}
	
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}









