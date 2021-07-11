/**
 * @file		motors.h
 * @author	Ben Grummitt
 * @brief		Header file for controlling the motors using a L298N H-Bridge
 */

#ifndef MOTORS_H
#define MOTORS_H

extern char currentMovement;

void initMotorDriver(void);
void driveMotors(char c);

#endif
