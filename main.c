#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tiva_c_led.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "application.h"
#include "tiva_c_uart4.h"


extern uint32_t SystemCoreClock;

TaskHandle_t led_notification_receiving_task = NULL, led_blink_task=NULL;

TaskData data[2];

int main()
{
	char *start_string = "Task Stats\n";
	char *task1_string = "Task 1 Prints\r\n", *task2_string = "Task 2 Prints\r\n";
	uint8_t len1 = strlen(task1_string), len2 = strlen(task2_string);
	uint32_t system_freq = 0;
	
	data[0].string = (uint8_t *)task1_string;
	data[0].length = len1;
	data[0].delay = 350;
	
	data[1].string = (uint8_t *)task2_string;
	data[1].length = len2;
	data[1].delay = 150;
	
	system_freq |= SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320, SystemCoreClock);
	
	//vInitializeQueue();
	
	vInitializeEventGroup();
	
	LEDPortInitialize();
	
	LEDInitialize();
	
	UART4PortKInitialize();
	
	UART4PortKGPIOInitialize();
	
	UART4Initialize();
	
	UART4InitializeInterrupt();
	
	UART4SendData((uint8_t *)start_string,strlen(start_string));															//Output on UART4
	
	while(1)
	{
		xTaskCreate(vLEDBlinkTask, "LED_BLINK", 200, NULL, 1, &led_blink_task);									//Blinking Task LED
		
		//xTaskCreate(vTask_Send, "Task1", 500, (void *)&data[0], 1, NULL);
		
		//xTaskCreate(vTask_Send, "Task2", 500, (void *)&data[1], 1, NULL);
		
		//xTaskCreate(vTerminalGatekeeperTask, "Task3", 1000, NULL, 2, NULL);
		
		//xTaskCreate(vLEDOnNotifyTask, "LED_EVENTS", 300, NULL, 1, NULL);
		
		//xTaskCreate(vLEDNotificationHandlerTask, "TASKNOTIFY_HDL", 300, NULL, 2, &led_notification_receiving_task);				//Just take the task handler into this variable
		
		xTaskCreate(vProcessReceiveMessage, "MsgRx", 300, NULL, 2, NULL);												//Process Message from UART Task
		
		xTaskCreate(vGetLEDTaskInfo, "LED_DIAG", 300, NULL, 2, NULL);														//Creating a task so that we can see in debug
		//xTaskCreate(vLEDBlinkTask, NULL, 200, NULL, 1, NULL);
		
		#if (configUSE_CO_ROUTINES > 0)
		xCoRoutineCreate(vLEDBlinkCoroutine, 0, 0);																	//Not using Index as of now, priority we can keep as 0
		#endif
		
		vTaskStartScheduler();
	}
}




// Lets try Application malloc failed hook





















