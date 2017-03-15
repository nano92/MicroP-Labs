/*******************************************************************************
  * @file    Thread_adc.c
  * @author  Luis Gallet
	* @version V1.0.0
  * @date    15-March-2016
  * @brief   	
  ******************************************************************************
  */

#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "Thread_adc.h"

void Thread_adc(void const *argument);             // thread function
osThreadId tid_Thread_adc;                         // thread id
osThreadDef(Thread_adc, osPriorityNormal, 1, 0);

HAL_StatusTypeDef StartADCHandle(ADC_HandleTypeDef *ADC1_Handle);
HAL_StatusTypeDef GetTempValue(ADC_HandleTypeDef* ADC1_Handle, uint32_t* ADC_value);
ADC_HandleTypeDef ADC1_Handle;
ADC_InitTypeDef ADC1_init;
ADC_ChannelConfTypeDef channel_config;
ADC_MultiModeTypeDef multi_mode_config;

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_adc(void){

  tid_Thread_adc = osThreadCreate(osThread(Thread_adc ), NULL); // Start LED_Thread
  if (!tid_Thread_adc) return(-1); 
  return(0);
}

/*----------------------------------------------------------------------------
 *      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
void Thread_adc(void const *argument){
	StartADCHandle(&ADC1_Handle);
	int32_t ADC_value = 0;
	while(1){
		osDelay(1000);
		GetTempValue(&ADC1_Handle, &ADC_value);
		//printf("adc val = %d\n", ADC_value);
	}
}

/* Function: StartADCHandle
 * Input   : ADC_HandleTypeDef *ADC1_Handle
 * Returns : HAL_StatusTypeDef status
 * Description : initiates the ADC and set ups the channel
 */
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

/* Function: GetTempValue
 * Input   : ADC_HandleTypeDef *ADC1_Handle
 * Output  : uint32_t* ADC_value
 * Returns : HAL_StatusTypeDef status
 * Description : Reads the value obtained in the ADC ADC1_Handle and stores it in ADC_value so that later 
 * function may use this value.
 */
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
	//printf("ADC temperature value: %u\n", *ADC_value);
	
	status = HAL_ADC_Stop(ADC1_Handle);
	if(status != HAL_OK){
		//printf("HAL_ADC_Stop status: %d\n", status);
		return status;
	}
	
	return status;
}