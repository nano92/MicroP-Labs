/**
  ******************************************************************************
  * File Name          : supporting_functions.h
  * Description        : User defined common functions that can be used across projects
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
#ifndef SUPPORTING_FUNCTIONS_H
#define SUPPORTING_FUNCTIONS_H

#include "stdint.h"
#include "gpio.h"
#include "lis3dsh.h"
#include <stdio.h>
#include <stdlib.h>

//Add as many definitions as you see necessary. 

//NO TWO DEFINITIONS SHOULD HAVE THE SAME ERROR CODE

#define RCC_CONFIG_FAIL       ((uint16_t) 0)
#define TIM_INIT_FAIL					((uint16_t) 1)
#define ADC_INIT_FAIL 				((uint16_t) 2)
#define ADC_CH_CONFIG_FAIL		((uint16_t) 3)
#define ADC_MULTIMODE_FAIL		((uint16_t) 4)
#define EXTI_SPI1_FAIL				((uint16_t) 5)

void Error_Handler						(uint16_t error_code);
void LIS3DSH_InterruptConfigStruct(LIS3DSH_DRYInterruptConfigTypeDef *LIS3DSH_IntConfigStruct);
void Init_NVIC_Interrupt(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void KeyBouncingDelay(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState pin_state, uint8_t rise_edge);
void Init_ACC();
void Init_Read_Keypad();

#endif
