/**
 * @file    Main.c
 * @author  Ben Grummitt
 * @brief   Main loop and system functions for running program
 */

#include <stdio.h>
#include "stm32f7xx_hal.h"
#include "Board_GLCD.h"
#include "Board_Touch.h"
#include "stm32746g_discovery_ts.h"

#include "motors.h"
#include "bluetoothConnection.h"
#include "obstacleDetector.h"
#include "GUI.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

void SystemClock_Config(void);

int main(void){
	TS_StateTypeDef touchState;
	
	HAL_Init(); //Init Hardware Abstraction Layer
	SystemClock_Config(); //Config Clocks
	GLCD_Initialize(); //Init GLCD
	Touch_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	
	drawProgramPage();
	
	initUARTBTRandom();
	initMotorDriver();
	initObstaclePins();

	driveMotors('S');
	
	for (;;){
		BSP_TS_GetState(&touchState);
		
		// If touch state is press down on screen
		if(touchState.touchDetected && touchState.touchEventId[0] == TOUCH_EVENT_PRESS_DOWN){
			pressedScreenProgram(touchState.touchX[0], touchState.touchY[0]);
		}
	}

}

#ifdef __RTX
extern uint32_t os_time;
uint32_t HAL_GetTick(void) {
	return os_time;
}
#endif

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();
	/* The voltage scaling allows optimizing the power
	consumption when the device is clocked below the
	maximum system frequency. */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/* Enable HSE Oscillator and activate PLL
	with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* Select PLL as system clock source and configure
	the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | 
	RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
