/**
 * @file		GUI.h
 * @author	Ben Grummitt
 * @brief		Header file for displaying and navigating the UI
 */

#ifndef GUI_H
#define GUI_H

#include <stdint.h>

// var for telling if car is running in programmed mode
extern uint8_t isRunning;
// var for telling gui if there has been a crash
extern uint8_t isCrash;

void drawProgramPage(void);
void pressedScreenProgram(int x, int y);
void drawDirectionsInBox(void);
void addDirectionAndRedraw(char dir);
void runDirections(void);

#endif
