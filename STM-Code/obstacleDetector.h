/**
 * @file		obstacleDetector.h
 * @author	Ben Grummitt
 * @brief		Header file for the obstacle detector 
 *					and active buzzer module
 */

#ifndef OBSTACLE_DETECTOR_H
#define OBSTACLE_DETECTOR_H

void initObstaclePins(void);
void EXTI9_5_IRQHandler(void);
void playBeep(void);

#endif
