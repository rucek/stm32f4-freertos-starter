#include "diag/Trace.h"

// board
#include "stm32f4xx.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void toggleLedWithTimer(void*);
void detectButtonPress(void*);
void toggleLedWithIpc(void*);
void initializeHardware();
void initializeGpioPin(GPIO_TypeDef *port, uint32_t pin, uint32_t mode);
void createTask(TaskFunction_t code, const char * const name);
void delayMillis(uint32_t millis);
void toggleLed(uint16_t pin);
GPIO_PinState readButtonState();

#define LED_PORT GPIOG
#define GREEN_LED_PIN GPIO_PIN_13
#define RED_LED_PIN GPIO_PIN_14

#define BUTTON_PORT GPIOA
#define BUTTON_PIN GPIO_PIN_0

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
		toggleLed(GREEN_LED_PIN);
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
		if (readButtonState() > 0) {
			/* Debounce */
			while (readButtonState() > 0)
				delayMillis(100);
			while (readButtonState() == 0)
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
			toggleLed(RED_LED_PIN);
		}
	}
}

void initializeHardware() {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
	initializeGpioPin(BUTTON_PORT, BUTTON_PIN, GPIO_MODE_INPUT);
	initializeGpioPin(LED_PORT, GREEN_LED_PIN | RED_LED_PIN, GPIO_MODE_OUTPUT_PP);

	HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
}

void initializeGpioPin(GPIO_TypeDef *port, uint32_t pin, uint32_t mode) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = pin;
	GPIO_InitStructure.Mode = mode;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(port, &GPIO_InitStructure);
}

void delayMillis(uint32_t millis) {
	/*
	 Delay for a period of time. vTaskDelay() places the task into
	 the Blocked state until the period has expired.
	 The delay period is spacified in 'ticks'. We can convert
	 yhis in milisecond with the constant portTICK_RATE_MS.
	 */
	vTaskDelay(millis / portTICK_RATE_MS);
}

void toggleLed(uint16_t pin) {
	HAL_GPIO_TogglePin(LED_PORT, pin);
}

GPIO_PinState readButtonState() {
	return HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);
}
