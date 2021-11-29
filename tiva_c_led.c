#include "tiva_c_led.h"
#include "inc/hw_memmap.h"							// HW Memory Map control
#include "driverlib/gpio.h"							// GPIO control
#include "driverlib/sysctl.h"						// System control


uint8_t LED_MASK[NUM_OF_LEDS] = {1<<LED1, 1<<LED2, 1<<LED3, 1<<LED4};


void LEDPortInitialize(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION) && !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));	
}



void LEDInitialize(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);										// LED at PORTF pin 0 and pin 4 -> 0x11
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);										// LED at PORTF pin 0 and pin 1 -> 0x03
}



void TurnOnLED(uint8_t leds)
{
	uint8_t idx=0;
	uint32_t led_port_base=0;
	
	for(; idx < NUM_OF_LEDS; ++idx)
	{
		
		led_port_base = (idx > 1) ? GPIO_PORTF_BASE : GPIO_PORTN_BASE;
		
		if(leds & (1<<idx))
			GPIOPinWrite(led_port_base, LED_MASK[idx], LED_MASK[idx]);
	}
}



void TurnOffLED(uint8_t leds)
{
	uint8_t idx=0;
	uint32_t led_port_base=0;
	
	for(; idx < NUM_OF_LEDS; ++idx)
	{
		
		led_port_base = (idx > 1) ? GPIO_PORTF_BASE : GPIO_PORTN_BASE;
		
		if(leds & (1<<idx))
			GPIOPinWrite(led_port_base, LED_MASK[idx], 0);
	}
}
