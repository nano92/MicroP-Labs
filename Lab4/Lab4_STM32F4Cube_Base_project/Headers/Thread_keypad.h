/**
  ******************************************************************************
  * File Name          : Thread_keypad.h
  * Description        : Thread_keypad function prototypes and shared variable
	* Author						 : Luis Gallet
	* Version            : 1.0.0
	* Date							 : March 3rd, 2017
  ****

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef Thread_keypad_H
#define Thread_keypad_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "stm32f4xx_hal.h"

extern char ANGLE_FLAG;

/* USER CODE BEGIN Prototypes */
int start_Thread_keypad (void);
osMessageQId getKeyPadMsgQueueId(void);
osMessageQId getStateMsgQueueId(void);
#endif