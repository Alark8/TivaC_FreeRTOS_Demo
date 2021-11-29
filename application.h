#ifndef APPLICATION_H
#define APPLICATION_H


#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "croutine.h"										// Coroutines header file

#define DELAY_TO_BE_USED														200
#define LENGTH_OF_QUEUE															  5


#define LED1_ON_MASK															(1 << 0)
#define LED2_ON_MASK															(1 << 1)
#define LED3_ON_MASK															(1 << 2)
#define LED4_ON_MASK															(1 << 3)
#define ALL_LED_OFF_MASK													(1 << 8)




typedef struct {
	uint8_t *string;
	uint8_t length;
	uint16_t delay;
}TaskData;



typedef struct{
	uint8_t *string;
	uint8_t length;
}QueueData;



void vInitializeQueue(void);
void vInitializeMutex(void);
void vInitializeEventGroup(void);
void vLEDBlinkTask(void *params);
void vTask_Send(void *params);
void vTerminalPrintTask(void *params);
void vTerminalGatekeeperTask(void *params);
void vLEDOnEventTask(void *params);
void vLEDEventHandlerTask(void *params);
void vProcessReceiveMessage(void *params);

void vLEDOnNotifyTask(void *params);
void vLEDNotificationHandlerTask(void *params);

#if(configUSE_CO_ROUTINES > 0)
void vApplicationIdleHook(void);
void vLEDBlinkCoroutine(CoRoutineHandle_t xHandle, UBaseType_t xIndex);
#endif

void vGetLEDTaskInfo(void *params);
void vApplicationMallocFailedHook(void);

#endif
