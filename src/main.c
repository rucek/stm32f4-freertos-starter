#include <stdio.h>
#include "diag/Trace.h"

// board
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void toggleLedWithTimer(void*);
void detectButtonPress(void*);
void toggleLedWithIpc(void*);
void initializeHardware();
void createTask(TaskFunction_t code, const char * const name);
void delayMillis(uint32_t millis);

xQueueHandle buttonPushesQueue;

int main(void) {
	trace_printf("Hello, ARM!\n");
	printf("Hello, printf\n");
	puts("Hello, puts\n");

	initializeHardware();

	/* Create IPC variables */
	buttonPushesQueue = xQueueCreate(10, sizeof(int));
	if (buttonPushesQueue == 0) {
		while (1)
			; /* fatal error */
	}

	/* Create tasks */
	createTask(toggleLedWithTimer, "Task1");
	createTask(detectButtonPress, "Task2");
	createTask(toggleLedWithIpc, "Task3");

	/* Start the RTOS Scheduler */
	vTaskStartScheduler();

	/* HALT */
	while (1)
		;
}

void createTask(TaskFunction_t code, const char * const name) {
	xTaskCreate(code, name, configMINIMAL_STACK_SIZE, (void *) NULL,
			tskIDLE_PRIORITY + 2UL, NULL);
}

/**
 * TASK 1: Toggle LED via RTOS Timer
 */
void toggleLedWithTimer(void *pvParameters) {
	while (1) {
		BSP_LED_Toggle(LED3);
		delayMillis(1500);
	}
}

/**
 * TASK 2: Detect Button Press And Signal Event via Inter-Process Communication (IPC)
 */
void detectButtonPress(void *pvParameters) {

	int sig = 1;

	while (1) {
		/* Detect Button Press  */
		if (BSP_PB_GetState(BUTTON_KEY) > 0) {
			/* Debounce */
			while (BSP_PB_GetState(BUTTON_KEY) > 0)
				delayMillis(100);
			while (BSP_PB_GetState(BUTTON_KEY) == 0)
				delayMillis(100);

			xQueueSendToBack(buttonPushesQueue, &sig, 0); /* Send Message */
		}
	}
}

/**
 * TASK 3: Toggle LED via Inter-Process Communication (IPC)
 */
void toggleLedWithIpc(void *pvParameters) {

	int sig;
	portBASE_TYPE status;

	while (1) {
		status = xQueueReceive(buttonPushesQueue, &sig, portMAX_DELAY); /* Receive Message */
		/* portMAX_DELAY blocks task indefinitely if queue is empty */
		if (status == pdTRUE) {
			BSP_LED_Toggle(LED4);
		}
	}
}

void initializeHardware() {
	HAL_Init();

	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

	BSP_LED_On(LED3);
	BSP_LED_Off(LED4);
}

void delayMillis(uint32_t millis) {
	/*
	 Delay for a period of time. vTaskDelay() places the task into
	 the Blocked state until the period has expired.
	 The delay period is specified in 'ticks'. We can convert
	 this in millisecond with the constant portTICK_RATE_MS.
	 */
	vTaskDelay(millis / portTICK_RATE_MS);
}
