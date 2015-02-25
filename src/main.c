#include "main.h"

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
	createTask(drawSmlLogo, "Logo");

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

void drawSmlLogo(void *pvParameters) {
	while (1) {
		BSP_LCD_Clear(LCD_COLOR_BLACK);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillCircle(100, 72, 10);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
		BSP_LCD_FillRect(90, 87, 60, 20);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
		BSP_LCD_FillCircle(140, 122, 10);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_DARKMAGENTA);
		BSP_LCD_FillRect(90, 137, 60, 20);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTMAGENTA);
		BSP_LCD_FillRect(80, 162, 60, 20);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
		BSP_LCD_FillRect(90, 187, 60, 20);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
		BSP_LCD_FillRect(90, 212, 60, 20);
		logoElementsDelay();
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_FillCircle(100, 247, 10);

		delayMillis(1000);
	}
}

void initializeHardware() {
	HAL_Init();

	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

	BSP_LED_On(LED3);
	BSP_LED_Off(LED4);

	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, ENABLE);
	BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
	BSP_LCD_DisplayOn();
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

/**
 * fixed SysTick interrupt handling
 * borrowed from https://github.com/dobromyslov/stm32f0-cmsis-cube-hal-freertos-template
 */
extern void xPortSysTickHandler(void);

void SysTick_Handler(void) {
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		xPortSysTickHandler();
	}
	HAL_IncTick();
}
