/**
 * @file	obastacleDetector.c
 * @author	Ben Grummitt
 * @brief	Functions for using an infared avoid sensor and active buzzer
 */

#include "stm32f7xx_hal.h"
#include "Board_GLCD.h"
#include <stdio.h>

#include "motors.h"
#include "GUI.h"
#include "bluetoothConnection.h"

/**
 * @brief Initialize GPIO pins for avoid sensor and buzzer
 */
void initObstaclePins(void){
	GPIO_InitTypeDef pinTypeDef;
	
	//PB8 - D15
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	pinTypeDef.Pin = GPIO_PIN_8;
	pinTypeDef.Mode = GPIO_MODE_IT_RISING_FALLING;
	pinTypeDef.Pull = GPIO_PULLDOWN;
	pinTypeDef.Speed = GPIO_SPEED_HIGH;
	
	HAL_GPIO_Init(GPIOB, &pinTypeDef);
	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	// PB9 - D14
	pinTypeDef.Pin = GPIO_PIN_9;
	pinTypeDef.Mode = GPIO_MODE_OUTPUT_PP;
	pinTypeDef.Pull = GPIO_NOPULL;
	pinTypeDef.Speed = GPIO_SPEED_HIGH;
	
	HAL_GPIO_Init(GPIOB, &pinTypeDef);
}

/**
 * @brief Interrupt handler for external interrupt 9-5
 */
void EXTI9_5_IRQHandler(void){
	int i;
	
	if(EXTI->PR & (1 << 8)){ // If pending register for pin 8 is set
		
		// If the car is moving forward and obstacle detected
		if(currentMovement == 'F'){
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET){
				driveMotors('S');
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
				
				if(isRunning){
					isCrash = 1;
				}else{
					sprintf(txBuffer, "!");
					writeBTMessage(1);
				}
			
				for(i = 0; i < 20000000; i++){}
				
				// Hal delay causes infinite delay and write wont stop
				// HAL_Delay(1000);
				
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
			}
		}
		
		// Clear Interrupt
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
	}

}

/**
 * @brief Function to play a short beep out of the buzzer
 */
void playBeep(void){
	int i;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	for(i = 0; i < 1000000; i++){}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
}

