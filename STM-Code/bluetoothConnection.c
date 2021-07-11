/**
 * @file    bluetoothConnection.c
 * @author  Ben Grummitt
 * @brief   Functions for initializing and communicating with a HC-06 bluetooth module
 *					over UART
 */

#include <stdio.h>
#include "stm32f7xx_hal.h"

#include "motors.h"
#include "GUI.h"

static UART_HandleTypeDef s_UARTHandle;
volatile char rxBuffer[4];
char txBuffer[9];

/**
 * @brief		Initialize RX and TX pins for UART
 *					then initialize UART to properties of HC-06
 */
void initUARTBTRandom(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__USART6_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	
	// USART6_TX PC6 D1 2
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	// USART6_RX PC7 D0 1
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	s_UARTHandle.Instance = USART6;
	s_UARTHandle.Init.BaudRate = 9600;
	s_UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
	s_UARTHandle.Init.StopBits = UART_STOPBITS_1;
	s_UARTHandle.Init.Parity = UART_PARITY_NONE;
	s_UARTHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	s_UARTHandle.Init.Mode = UART_MODE_TX_RX;
	
	if(HAL_UART_Init(&s_UARTHandle) != HAL_OK){
		// ERRROR ????
	}
	
	NVIC_EnableIRQ(USART6_IRQn);
	
	sprintf(txBuffer, "Conn");
	
	HAL_UART_Receive_IT(&s_UARTHandle, (uint8_t*)rxBuffer, sizeof(rxBuffer));
	
	HAL_UART_Transmit_IT(&s_UARTHandle, (uint8_t *)txBuffer, 4 * sizeof(uint8_t));
	
}

/**
 * @brief Function called when there is an interrupt from recieving
 *				data from UART
 */
void USART6_IRQHandler(void){
	volatile unsigned int ISR; // Interupt status register
	volatile unsigned char c;
	
	ISR = USART6->ISR;
	
	// Mask to check if data register is not empty
	// Recieving multiple chars at once can cause OverRun Error
	if(ISR & USART_ISR_RXNE){
		c = USART6->RDR;

		USART6->ISR &= ~(USART_ISR_RXNE); // Clear data present register
		
		// If the car is not running a pre programmed sequence drive in sent direction
		if(!isRunning){
			driveMotors(c);
		}
	}else{
		HAL_UART_IRQHandler(&s_UARTHandle);
	}
	
}

/**
 * @brief Function to write the data from txBuffer to the UART port
 * @param numChars Number of chars to output to UART TX pin (max 8)
 * 				if more than 8 is passed will just output whole buffer
 */
void writeBTMessage(int numChars){
	if(numChars <= 8){
		HAL_UART_Transmit_IT(&s_UARTHandle, (uint8_t *)txBuffer, numChars * sizeof(uint8_t));
	}else{
		HAL_UART_Transmit_IT(&s_UARTHandle, (uint8_t *)txBuffer, sizeof(txBuffer));
	}
}
