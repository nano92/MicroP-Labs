/*******************************************************************************
  * @file    Thread_LED.c
  * @author  Ashraf Suyyagh
	* @version V1.0.0
  * @date    17-January-2016
  * @brief   This file initializes one LED as an output, implements the LED thread 
  *					 which toggles and LED, and function which creates and starts the thread	
  ******************************************************************************
  */
	
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "timer.h"
void Thread_LED (void const *argument);                 // thread function
osThreadId tid_Thread_LED;                              // thread id
osThreadDef(Thread_LED, osPriorityNormal, 1, 0);
GPIO_InitTypeDef 				LED_configuration;

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_LED (void) {

  tid_Thread_LED = osThreadCreate(osThread(Thread_LED ), NULL); // Start LED_Thread
  if (!tid_Thread_LED) return(-1); 
  return(0);
}

 /*----------------------------------------------------------------------------
*      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
	void Thread_LED (void const *argument) {
		
		while(1){
				osDelay(1000);
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			}
	}
/*----------------------------------------------------------------------------
 *      Initialize the GPIO associated with the LED
 *---------------------------------------------------------------------------*/
	void initializeLED_IO (void){
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	LED_configuration.Pin		= GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	LED_configuration.Mode 	= GPIO_MODE_OUTPUT_PP;
	LED_configuration.Speed	= GPIO_SPEED_FREQ_VERY_HIGH;
	LED_configuration.Pull	= GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &LED_configuration);	
}
	
void alarm() {
	// Takes care of dislpaying the alarm (this is what is done in the previous lab) 
	// 	const uint16_t LED_array[4] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};
//	if(temp_alarm){
//		HAL_GPIO_TogglePin(GPIOD, LED_array[led]);
//		if (temp_counter == 16) {
//				temp_counter = 0;
//				led = (led == 4) ? 0 : led+1;
//		} else {
//				temp_counter += 1;
//		}
//	}else{
//		for(int8_t i = 0; i < 4; i++){
//			HAL_GPIO_WritePin(GPIOD, LED_array[i], GPIO_PIN_RESET);
//		}
//	}
}
	
/*----------------------------------------------------------------------------
 *      
 *---------------------------------------------------------------------------*/