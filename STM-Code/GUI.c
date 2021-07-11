/**
 * @file    GUI.c
 * @author  Ben Grummitt
 * @brief   Functions drawing to the GLCD and taking touch input locations for buttons
 */

#include <stdio.h>
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "stm32f7xx_hal.h"

#include "motors.h"
#include "obstacleDetector.h"

void drawProgramPage(void);
void pressedScreenProgram(int x, int y);
void drawDirectionsInBox(void);
void addDirectionAndRedraw(char dir);
void runDirections(void);
void removeLatestDirection(void);

void resetDirections(void);

#define SCREEN_HEIGHT 270
#define SCREEN_WIDTH 480

#define SCREEN_HOME 0
#define SCREEN_BLUETOOTH 1
#define SCREEN_PROGRAM 2

#define NUM_DIRECTIONS 10

#define BOX_WIDTH 180
#define BOX_HEIGHT 60
#define BOX_INFO_WIDTH 100

#define BOX_START_LEFT ((SCREEN_WIDTH / 2) - (BOX_WIDTH / 2))
#define BOX_START_TOP (SCREEN_HEIGHT / 4)

char directions[NUM_DIRECTIONS];
int numProgrammed = 0;

volatile uint8_t isRunning = 0;
volatile uint8_t isCrash = 0;


/**
 * @brief	Function to use the direction inputted and then drive car using directions
 */
void runDirections(void){
	int i;
	
	isRunning = 1;
	
	for(i = 0; i < numProgrammed; i++){
		driveMotors(directions[i]);
		HAL_Delay(1000);
		if(isCrash){
			break;
		}
	}
	
	if(!isCrash){
		resetDirections();
		drawDirectionsInBox();
	}
	
	driveMotors('S');
	
	isRunning = 0;
	isCrash = 0;
}

/**
 * @brief Function Called When The Program Wants To Draw The Screen For Programming The Car
 */
 void drawProgramPage(void){
	
	// Draw Title
	GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
	GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
	GLCD_DrawString(155, 1*24, "Program Car");
	
	// Draw Direction Buttons
	GLCD_DrawRectangle(1.8*24 - 5, ((SCREEN_HEIGHT * 3.2) / 7) + 17, 24, 29);
	GLCD_DrawString(1.8*24, ((SCREEN_HEIGHT * 3.2) / 7) + 22, "L");
	GLCD_DrawRectangle(4*24 - 5, ((SCREEN_HEIGHT * 3.2) / 7) + 17, 24, 29);
	GLCD_DrawString(4*24, ((SCREEN_HEIGHT * 3.2) / 7) + 22, "S");
	GLCD_DrawRectangle(6.2*24 - 5, ((SCREEN_HEIGHT * 3.2) / 7) + 17, 24, 29);
	GLCD_DrawString(6.2*24, ((SCREEN_HEIGHT * 3.2) / 7) + 22, "R");
	GLCD_DrawRectangle(4*24 - 5, ((SCREEN_HEIGHT * 3.2) / 7) + 17 - 2.2*24, 24, 29);
	GLCD_DrawString(4*24, ((SCREEN_HEIGHT * 3.2) / 7) + 22 - 2.2*24, "F");
	GLCD_DrawRectangle(4*24 - 5, ((SCREEN_HEIGHT * 3.2) / 7) + 17 + 2.2*24, 24, 29);
	GLCD_DrawString(4*24, ((SCREEN_HEIGHT * 3.2) / 7) + 22 + 2.2*24, "B");
	
	// Draw Box For Directions
	GLCD_DrawRectangle(8 * 24, 3 * 24, (SCREEN_WIDTH - 9*24), 156);
	
	// Draw Go Rectangl
	GLCD_DrawRectangle(8 * 24, SCREEN_HEIGHT - 1.6 * 24, 4*24, 1.5*24);
	GLCD_DrawString(9.4 * 24, SCREEN_HEIGHT - 1.2 * 24, "GO");
	
	// Draw Clear
	GLCD_DrawRectangle(15 * 24, SCREEN_HEIGHT - 1.6 * 24, 4*24, 1.5*24);
	GLCD_DrawString(15.4 * 24, SCREEN_HEIGHT - 1.2 * 24, "Clear");
}

/**
 * @brief Function Called When A Button Is Pressed In The Program Screen
 * @param	x Position in the x axis of the touch
 * @param y Position in the y axis of the touch
 */
void pressedScreenProgram(int x, int y){
	
	// Bottom Button Row
	if(y >= (SCREEN_HEIGHT - 1.6 * 24) && y <= (SCREEN_HEIGHT - 1.6 * 24) + 1.5 * 24){
		// Go Button
		if(x >= 8*24 && x <= 12*24){
			playBeep();
			runDirections();
		}
		
		// Clear Button
		if(x >= 15 * 24 && x <= 19 * 24){
			resetDirections();
			drawDirectionsInBox();
			playBeep();
		}
	}
	
	// Centre column of direction buttons
	if(x >= 4*24 && x <= (4*24)+24){
		// Pressed F
		if(y >= (((SCREEN_HEIGHT * 3.2) / 7) + 22 - 2.2*24) && y <= (((SCREEN_HEIGHT * 3.2) / 7) + 22 - 2.2*24) + 29){
			addDirectionAndRedraw('F');
		}
		
		// Pressed S
		if(y >= ((SCREEN_HEIGHT * 3.2) / 7) + 17 && y <= (((SCREEN_HEIGHT * 3.2) / 7) + 17) + 29){
			addDirectionAndRedraw('S');
		}
		
		// Pressed B
		if(y >= ((SCREEN_HEIGHT * 3.2) / 7) + 17 + 2.2*24 && y <= ((SCREEN_HEIGHT * 3.2) / 7) + 17 + 2.2*24 + 29){
			addDirectionAndRedraw('B');
		}
	}
	
	// Centre row of direction buttons
	if(y >= ((SCREEN_HEIGHT * 3.2) / 7) + 17 && y <= ((SCREEN_HEIGHT * 3.2) / 7) + 17 + 29){
		//Pressed L
		if(x >= 1.8*24 && x <= 1.8*24 + 24){
			addDirectionAndRedraw('L');
		}
		
		// Pressed R
		if(x >= 6.2*24 - 5 && x <= 6.2*24 - 5 + 24){
			addDirectionAndRedraw('R');
		}
	}
}

/**
 * @brief Function Called When The Program Wants Add A Direction To The Programming Mode
 * @param c Direction to be added the directions list F=Forward L=Left R=Right B=Backwards S/Else=Stop
 */
void addDirectionAndRedraw(char c){
	if(numProgrammed < NUM_DIRECTIONS){
		directions[numProgrammed++] = c;
	}
	
	drawDirectionsInBox();
	playBeep();
}

/**
 * @brief Function Called When The Program Wants Draw The Programmed Directions In Its Box
 */
void drawDirectionsInBox(void){
	int i, offset;
	
	if(numProgrammed >= 6){
		offset = numProgrammed - 6;
	}else{
		offset = 0;
	}
	
	for(i = 0; i < 6; i++){
		if(i < numProgrammed){
			switch(directions[i + offset]){
				case 'F':
					GLCD_DrawString((8 * 24) + 1, 3 * 24 + 8 + i * 24, "FORWARDS ");
					break;
				case 'B':
					GLCD_DrawString((8 * 24) + 1, 3 * 24 + 8 + i * 24, "BACKWARDS");
					break;
				case 'L':
					GLCD_DrawString((8 * 24) + 1, 3 * 24 + 8 + i * 24, "LEFT     ");
					break;
				case 'R':
					GLCD_DrawString((8 * 24) + 1, 3 * 24 + 8 + i * 24, "RIGHT    ");
					break;
				default:
					GLCD_DrawString((8 * 24) + 1, 3 * 24 + 8 + i * 24, "STOP     ");
			}
		}else{
			GLCD_DrawString((8 * 24) + 1, 3 * 24 + 8 + i * 24, "         ");
		}
	}
}


/**
 * @brief Remove All Directions From Array
 */
void resetDirections(void){
	numProgrammed = 0;
}

/**
 * @brief Function To Remove The Last Added Direction To The Array
 */
void removeLatestDirection(void){
	if(numProgrammed > 0){
		numProgrammed--;
	}
}
