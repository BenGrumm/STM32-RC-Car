/**
 * @file		bluetoothConnection.h
 * @author	Ben Grummitt
 * @brief		Header file for UART communication with HC-06
 */

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

extern char txBuffer[9];

void initUARTBTRandom(void);
void USART6_IRQHandler(void);
void writeBTMessage(int numChars);

#endif
