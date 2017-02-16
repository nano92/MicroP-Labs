/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

ADC_InitTypeDef ADC1_init;
ADC_ChannelConfTypeDef channel_config;
ADC_MultiModeTypeDef multi_mode_config;

HAL_StatusTypeDef StartADCHandle(ADC_HandleTypeDef *ADC1_Handle){
	HAL_StatusTypeDef status;
	
	__HAL_RCC_ADC1_CLK_ENABLE(); //Enable ADC1 GPIO clock
	
	
	ADC1_Handle->Instance = ADC1;
	ADC1_Handle->Init = ADC1_init;
	ADC1_Handle->Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
	ADC1_Handle->Init.Resolution = ADC_RESOLUTION12b;
	ADC1_Handle->Init.DataAlign = ADC_DATAALIGN_LEFT;
	ADC1_Handle->Init.ScanConvMode = DISABLE;
	ADC1_Handle->Init.EOCSelection = ADC_EOC_SEQ_CONV;
	ADC1_Handle->Init.ContinuousConvMode = DISABLE;
	ADC1_Handle->Init.DMAContinuousRequests = DISABLE;
	ADC1_Handle->Init.NbrOfConversion = 1;
	ADC1_Handle->Init.DiscontinuousConvMode = DISABLE;
	ADC1_Handle->Init.NbrOfDiscConversion = 1;
	
	status = HAL_ADC_Init(ADC1_Handle);
	if(status != HAL_OK){
		printf("HAL_ADC_Init status: %d\n", status);
		return status;
	}
	
	channel_config.Channel = ADC_CHANNEL_TEMPSENSOR;
	channel_config.Rank = 1;
	channel_config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	
	multi_mode_config.Mode = ADC_MODE_INDEPENDENT;
	multi_mode_config.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_20CYCLES;
	
	status = HAL_ADC_ConfigChannel(ADC1_Handle, &channel_config);
	if(status != HAL_OK){
		printf("HAL_ADC_ConfigChannel status: %d\n", status);
		return status;
	}
	
	status = HAL_ADCEx_MultiModeConfigChannel(ADC1_Handle, &multi_mode_config);
	if(status != HAL_OK){
		printf("HAL_ADCEx_MultiModeConfigChannel status: %d\n", status);
		return status;
	}
	
	return status;
}

HAL_StatusTypeDef GetTempValue(ADC_HandleTypeDef* ADC1_Handle, uint32_t* ADC_value){
	//uint32_t temp_value = 0;
	HAL_StatusTypeDef status;
		
	status = HAL_ADC_Start(ADC1_Handle);
	if(status != HAL_OK){
		printf("HAL_ADC_Start status: %d\n", status);
		return status;
	}
	
	status = HAL_ADC_PollForConversion(ADC1_Handle, 10);
	if(status != HAL_OK){	
		printf("HAL_ADC_PollForConversion status: %d\n", status);
		return status;
	}
	
	*ADC_value = HAL_ADC_GetValue(ADC1_Handle);
	printf("ADC temperature value: %u\n", *ADC_value);
	
	status = HAL_ADC_Stop(ADC1_Handle);
	if(status != HAL_OK){
		//printf("HAL_ADC_Stop status: %d\n", status);
		return status;
	}
	
	return status;
}
/* USER CODE END 0 */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
