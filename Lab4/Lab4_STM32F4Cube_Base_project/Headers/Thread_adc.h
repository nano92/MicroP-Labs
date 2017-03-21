/**
  ******************************************************************************
  * File Name          : Thread_adc.h
  * Description        : Thread_adc function prototypes
	* Author						 : Luis Gallet
	* Version            : 1.0.0
	* Date							 : March 3rd, 2017
  ****
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __7segmentGpio_H
#define __7segmentGpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
int start_Thread_adc(void);
osMessageQId getADCMsgQueueId(void);
osMessageQId getAlarmMsgQueueId(void);
osThreadId getADCThreadId(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */