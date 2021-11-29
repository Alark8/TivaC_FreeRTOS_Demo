#include "application.h"
#include "tiva_c_led.h"
#include "semphr.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "queue.h"                      // ARM.FreeRTOS::RTOS:Core
#include "tiva_c_uart4.h"
#include "stream_buffer.h"              // ARM.FreeRTOS::RTOS:Stream Buffer


SemaphoreHandle_t mutex_handle = NULL;
QueueHandle_t queue_handle = NULL;
EventGroupHandle_t led_events = NULL;

extern StreamBufferHandle_t uart_stream_buffer;
extern TaskHandle_t led_notification_receiving_task, led_blink_task;

void vInitializeMutex(void)
{
	mutex_handle = xSemaphoreCreateMutex();
	
	if (mutex_handle == NULL)
	{
		// Just do any action
	}
}



void vInitializeQueue(void)
{
	queue_handle = xQueueCreate(LENGTH_OF_QUEUE, sizeof(QueueData));
	
	if (queue_handle == NULL)
	{
		// Just do any action
	}
}



void vInitializeEventGroup(void)
{
	led_events = xEventGroupCreate();
	
	if (led_events == NULL)
	{
		// Just do any action
	}
}


void vLEDBlinkTask(void *params)
{
	uint32_t loop_val=0;
	
	while(1)
	{
		TurnOnLED(0x08);				//Turn on LED4 as LED1 will be turned on by some other function
		
		for(; loop_val < 2000000; ++loop_val);								//This loop is just to execute this specific block for a long time
		
		vTaskDelay(pdMS_TO_TICKS(DELAY_TO_BE_USED));
		
		TurnOffLED(0x08);				//Turn OFF LED4 as LED1 will be turned of by some other function
		
		for(loop_val=0; loop_val < 2000000; ++loop_val);								//This loop is just to execute this specific block for a long time
		
		vTaskDelay(pdMS_TO_TICKS(DELAY_TO_BE_USED));
	}
}


/* LED1 blinks, indicating that as per the timings the MCU is going into IDLE Mode (low power) */




void vTerminalPrintTask(void *params)
{
	TaskData *data = (TaskData *)params;
	
	while(1)
	{
		xSemaphoreTake(mutex_handle, portMAX_DELAY);
		{
			UART4SendData(data->string, data->length);
		}
		xSemaphoreGive(mutex_handle);
		
		vTaskDelay(pdMS_TO_TICKS(data->delay));
	}
}





void vTask_Send(void *params)
{
	
	TaskData *data = (TaskData *)params;
	QueueData queue_data;
	
	queue_data.string = data->string;
	queue_data.length = data->length;
	
	while(1)
	{
		xQueueSend(queue_handle, &queue_data, 0);
		
		vTaskDelay(pdMS_TO_TICKS(data->delay));
	}
}




void vTerminalGatekeeperTask(void *params)
{
	
	QueueData data;
	
	while(1)
	{
		xQueueReceive(queue_handle, &data, portMAX_DELAY);
		
		UART4SendData(data.string, data.length);
	}
}



void vLEDOnEventTask(void *params)
{
	while(1)
	{
		xEventGroupSetBits(led_events, LED1_ON_MASK);
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xEventGroupSetBits(led_events, LED2_ON_MASK);
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xEventGroupSetBits(led_events, LED3_ON_MASK);
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xEventGroupSetBits(led_events, LED4_ON_MASK);
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xEventGroupSetBits(led_events, ALL_LED_OFF_MASK);
		
		vTaskDelay(pdMS_TO_TICKS(300));
	}
}




void vLEDEventHandlerTask(void *params)
{
	BaseType_t event_group_value=0;
	
	while(1)
	{
		event_group_value = xEventGroupWaitBits(led_events, LED1_ON_MASK | LED2_ON_MASK | LED3_ON_MASK | LED4_ON_MASK | ALL_LED_OFF_MASK, pdTRUE, pdFALSE, portMAX_DELAY);
		
		if( (event_group_value & LED1_ON_MASK) != 0)
			TurnOnLED(0x01);
		
		if( (event_group_value & LED2_ON_MASK) != 0)
			TurnOnLED(0x02);
		
		if( (event_group_value & LED3_ON_MASK) != 0)
			TurnOnLED(0x04);
		
		if( (event_group_value & LED4_ON_MASK) != 0)
			TurnOnLED(0x08);
		
		if( (event_group_value & ALL_LED_OFF_MASK) != 0)
			TurnOffLED(0x0F);
	}
}




void vProcessReceiveMessage(void *params)
{
	size_t bytes_received=0;
	uint8_t data[LENGTH_OF_DATA] = {(0)};							// Creating a static array right now, we can also create a dynamic one
	
	while(1)
	{
		bytes_received |= xStreamBufferReceive(uart_stream_buffer, data, LENGTH_OF_DATA, portMAX_DELAY);			
		//1st Arg - buffer handle, 2nd Arg - data holder, 3rd Arg - Length of data, 4th Arg - blocking time
		
		UART4SendData(data, LENGTH_OF_DATA);										//Send the data back over the same UART or any other UART.
	}
}


/* The flow should have come here but it didn't ! Lets check where it is stuck */

/* Now it didn't stuck here */


#if(configUSE_CO_ROUTINES > 0)													// Just made the coroutine work like this
void vApplicationIdleHook(void)
{
	while(1)
	{
		vCoRoutineSchedule();																//As our idle task will do nothing, we can call vCoRoutineSchedule in a while loop
	}
}



void vLEDBlinkCoroutine(CoRoutineHandle_t xHandle, UBaseType_t xIndex)
{
	crSTART(xHandle);
	
	while(1)
	{
		TurnOnLED(0x0F);								//Turning On all LEDs
		
		crDELAY(xHandle, pdMS_TO_TICKS(200));															//This is the delay function that can be used in coroutine
		
		TurnOffLED(0x0F);								//Turning Off all LEDs
		
		crDELAY(xHandle, pdMS_TO_TICKS(200));															//This is the delay function that can be used in coroutine
	}
	
	crEND();
}
#endif



/* Lets try an example of task notification using xTaskNotify and xTaskNotifyWait */


void vLEDOnNotifyTask(void *params)																				//LED ON Function making use of Task Notification
{
	while(1)
	{
		xTaskNotify(led_notification_receiving_task, LED1_ON_MASK, eSetBits);	//1st Arg - task handler to which we wish to send notification, 2nd Arg - bits we want to set, 3rd Arg - eAction
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xTaskNotify(led_notification_receiving_task, LED2_ON_MASK, eSetBits);	//1st Arg - task handler to which we wish to send notification, 2nd Arg - bits we want to set, 3rd Arg - eAction
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xTaskNotify(led_notification_receiving_task, LED3_ON_MASK, eSetBits);	//1st Arg - task handler to which we wish to send notification, 2nd Arg - bits we want to set, 3rd Arg - eAction
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xTaskNotify(led_notification_receiving_task, LED4_ON_MASK, eSetBits);	//1st Arg - task handler to which we wish to send notification, 2nd Arg - bits we want to set, 3rd Arg - eAction
		
		vTaskDelay(pdMS_TO_TICKS(300));
		
		xTaskNotify(led_notification_receiving_task, ALL_LED_OFF_MASK, eSetBits);	//1st Arg - task handler to which we wish to send notification, 2nd Arg - bits we want to set, 3rd Arg - eAction
		
		vTaskDelay(pdMS_TO_TICKS(300));
	}
}




void vLEDNotificationHandlerTask(void *params)				//LED Notification handler that will make use of notifications instead of event groups
{
	BaseType_t result=0;
	uint32_t notification_value=0;
	
	while(1)
	{
		result = xTaskNotifyWait(notification_value, pdFALSE, &notification_value, portMAX_DELAY);				// Wait for infinite time here.
		/* We have made use of clear on entry whereas the bits to clear are specified by notification value, try to understand why ! */	
		
		if( (notification_value & LED1_ON_MASK) != 0)
			TurnOnLED(0x01);
		
		if( (notification_value & LED2_ON_MASK) != 0)
			TurnOnLED(0x02);
		
		if( (notification_value & LED3_ON_MASK) != 0)
			TurnOnLED(0x04);
		
		if( (notification_value & LED4_ON_MASK) != 0)
			TurnOnLED(0x08);
		
		if( (notification_value & ALL_LED_OFF_MASK) != 0)
			TurnOffLED(0x0F);
	}
}


/* The MCU is performing as it did with the event groups ! */







void vApplicationMallocFailedHook(void)
{
	char *string = "Bad Memory Request";
	
	TurnOnLED(0x01);						//Turning On LED1 as Indication of faulty memory request
	
	UART4SendData((uint8_t *)string, strlen(string));							//Just sending out debug information
}




void vGetLEDTaskInfo(void *params)
{
	TaskStatus_t task_status;
	
	while(1)
	{
		vTaskGetTaskInfo(led_blink_task, &task_status, pdTRUE, NULL);							//1st Arg - task handler for which we want to know, 2nd Arg - pointer to task structure, 3rd Arg - Set true to
																																							// get high water mark level of stack, 4th Arg - Not using this 
		
		vTaskDelay(pdMS_TO_TICKS(2000));																					//Just 2 seconds delay
	}
}

//Debugger has stopped

//Stack high water mark level is at 0xB1 that means out of 200 bytes, the function has utilized (200 - 0xB1) bytes in stack only.







