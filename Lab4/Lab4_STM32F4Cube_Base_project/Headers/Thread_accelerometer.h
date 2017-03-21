/**
  ******************************************************************************
  * File Name          : Thread_accelerometer.h
  * Description        : Reading of the accelerometer values
	* Authors						 : Juan Carlos Borges, Luis Gallet
  * Group              : 10	
	* Version            : 1.0.0
	* Date							 : March 10th, 2017
  ******************************************************************************
  */
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "stm32f4xx_hal.h"
#include "supporting_functions.h"


#define PI 3.14159265358979323846

void readingACC(float *values);
int start_Thread_keypad (void);
void StartLEDGPIO(void);
void InitAccGPIO(void);
osMessageQId getPitchACCMsgQueueId(void);
osMessageQId getRollACCMsgQueueId(void);
osThreadId getACCThreadId(void);
#endif