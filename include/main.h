#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include "diag/Trace.h"

// board
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_sdram.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void toggleLedWithTimer(void*);
void detectButtonPress(void*);
void toggleLedWithIpc(void*);
void drawSmlLogo(void *);
void initializeHardware();
void createTask(TaskFunction_t code, const char * const name);
void delayMillis(uint32_t millis);

#define logoElementsDelay() delayMillis(500);

#endif
