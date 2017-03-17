/**
  ******************************************************************************
  * File Name          : timer.h
  * Description        : TIM4 configuration and related functions file
	* Author						 : Luis Gallet
	* Version            : 1.0.0
	* Date							 : March 3rd, 2017
  ******************************************************************************
  */
#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx_hal.h"
#include "supporting_functions.h"

//extern TIM_HandleTypeDef handle_tim;

HAL_StatusTypeDef Init_TIM4_Config(TIM_HandleTypeDef *handle_tim4);
HAL_StatusTypeDef Init_TIM3_Config(TIM_HandleTypeDef *handle_time3);
HAL_StatusTypeDef Set_LEDBrightness(int16_t acc_roll_angle, int16_t acc_pitch_angle, int16_t roll_angle, int16_t pitch_angle, TIM_HandleTypeDef *handle_tim4);
#endif