/**
 * @file    motors.c
 * @author  Ben Grummitt
 * @brief   Functions for initializing and communicating with a DC motors
 *					using an L298N H-Bridge
 */

#include <stdio.h>
#include "stm32f7xx_hal.h"

void setLeftMotors(GPIO_PinState pin1, GPIO_PinState pin2);
void setRightMotors(GPIO_PinState pin3, GPIO_PinState pin4);
GPIO_InitTypeDef motorDriverPins[4];

char currentMovement = 'S';

/**
 * @brief		Initialize GPIO pins for output
 */
void initMotorDriver(void){
	int i;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
	
	for(i = 0; i < sizeof(motorDriverPins) / sizeof(GPIO_InitTypeDef); i++){
		motorDriverPins[i].Mode = GPIO_MODE_OUTPUT_PP;
		motorDriverPins[i].Pull = GPIO_NOPULL;
		motorDriverPins[i].Speed = GPIO_SPEED_HIGH;
	}
	
	motorDriverPins[0].Pin = GPIO_PIN_3; 	// I3 - D7
	motorDriverPins[1].Pin = GPIO_PIN_2; 	// I2 - D8
	motorDriverPins[2].Pin = GPIO_PIN_15;	// A15 - D9
	motorDriverPins[3].Pin = GPIO_PIN_8;	// A8 - D10
	
	HAL_GPIO_Init(GPIOI, &motorDriverPins[0]);
	HAL_GPIO_Init(GPIOI, &motorDriverPins[1]);
	HAL_GPIO_Init(GPIOA, &motorDriverPins[2]);
	HAL_GPIO_Init(GPIOA, &motorDriverPins[3]);
}

/**
 * @brief	Function to drive motors in given direction
 *				F = Forward
 *				R = Right
 *				L = Left
 *				B = Backwards
 *				S / Else = Stop
 *				Forward = High(1) Low(0)
 *				Backward = Low(0) High(1)
 *				Off = Low(0) Low(0) or High(1) High(1)
 * @param	direction Direction of which to drive the car in
 */
void driveMotors(char direction){
	switch(direction){
		case 'L':
			// Drive Left
			// Spin Left Side Backward
			setLeftMotors(GPIO_PIN_SET, GPIO_PIN_RESET);
			// Spin Right Side Forward
			setRightMotors(GPIO_PIN_RESET, GPIO_PIN_SET);
			break;
		case 'R':
			// Drive Right
			// Spin Right Side Backward
			setRightMotors(GPIO_PIN_SET, GPIO_PIN_RESET);
			// Sping Left Side Forward
			setLeftMotors(GPIO_PIN_RESET, GPIO_PIN_SET);
			break;
		case 'F':
			// Drive Forward
			setLeftMotors(GPIO_PIN_SET, GPIO_PIN_RESET);
			setRightMotors(GPIO_PIN_SET, GPIO_PIN_RESET);
			break;
		case 'B':
			// Drive Backwards
			setRightMotors(GPIO_PIN_RESET, GPIO_PIN_SET);
			setLeftMotors(GPIO_PIN_RESET, GPIO_PIN_SET);
			break;
		default:
			// Stop
			setLeftMotors(GPIO_PIN_RESET, GPIO_PIN_RESET);
			setRightMotors(GPIO_PIN_RESET, GPIO_PIN_RESET);
	}
	currentMovement = direction;
}

/**
 * @brief Function to drive the motors on the left side
 * @param pin1 The state to set pin 1 that controls the left motors
 * @param pin2 The state to set pin 2 that controls the left motors
 */
void setLeftMotors(GPIO_PinState pin1, GPIO_PinState pin2){
	HAL_GPIO_WritePin(GPIOI, motorDriverPins[0].Pin, pin1);
	HAL_GPIO_WritePin(GPIOI, motorDriverPins[1].Pin, pin2);
}


/**
 * @brief Function to drive the motors on the right side
 * @param pin3 The state to set pin 3 that controls the left motors
 * @param pin4 The state to set pin 4 that controls the left motors
 */
void setRightMotors(GPIO_PinState pin3, GPIO_PinState pin4){
	HAL_GPIO_WritePin(GPIOA, motorDriverPins[2].Pin, pin3);
	HAL_GPIO_WritePin(GPIOA, motorDriverPins[3].Pin, pin4);
}
