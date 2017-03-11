/**
  ******************************************************************************
  * File Name          : accelerometer.h
  * Description        : Reading of the accelerometer values
	* Authors						 : Juan Carlos Borges, Luis Gallet
  * Group              : 10	
	* Version            : 1.0.0
	* Date							 : March 10th, 2017
  ******************************************************************************
  */
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define PI 3.14159265358979323846

#include "stm32f4xx_hal.h"
#include "supporting_functions.h"


//extern TIM_HandleTypeDef handle_tim;
float readingACC();
#endif