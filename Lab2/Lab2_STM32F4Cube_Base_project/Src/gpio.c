/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
GPIO_InitTypeDef GPIOD_init;
GPIO_InitTypeDef GPIOA_init;

void Start7SegmentDisplayGPIO(){
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIOD_init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
										| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIOA_init.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	
	GPIOD_init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOA_init.Mode = GPIO_MODE_OUTPUT_PP;
	
	GPIOD_init.Pull = GPIO_PULLDOWN;
	GPIOA_init.Pull = GPIO_PULLDOWN;
	
	GPIOD_init.Speed = GPIO_SPEED_FREQ_LOW ;
	GPIOA_init.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOD, &GPIOD_init);
	HAL_GPIO_Init(GPIOA, &GPIOA_init);
}

void DisplayTemperature(){
	
	char command[4][8] = {"11111101","00010010","11000000","11111001"};//-5.01
	
	uint16_t GPIOD_array[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, 
															GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6 , GPIO_PIN_7};
	
	uint16_t GPIOA_array[4] = {GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};
	
	
	for(int8_t i = 0; i < 4; i++){
		HAL_GPIO_WritePin(GPIOA, GPIOA_array[i], GPIO_PIN_SET);
	}
	
	for(int8_t n = 0; n < 4; n++) {
		
		HAL_GPIO_WritePin(GPIOA, GPIOA_array[n], GPIO_PIN_RESET);
		
		for(int8_t i = 0; i < 8; i++){
			if(command[n][i] == '1'){
				//printf("%c", command[n][i]);
				HAL_GPIO_WritePin(GPIOD, GPIOD_array[i], GPIO_PIN_RESET);
			}else{
				//printf("%c", command[n][i]);
				HAL_GPIO_WritePin(GPIOD, GPIOD_array[i], GPIO_PIN_SET);
			}

		}
		HAL_GPIO_WritePin(GPIOA, GPIOA_array[n], GPIO_PIN_SET);		//printf("\n");	
	}
}
/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PXX   ------> YYYYYY
		 
		 Continue your GPIO configuration information as abow.

*/

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
