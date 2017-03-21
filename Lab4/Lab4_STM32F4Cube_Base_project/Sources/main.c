/*******************************************************************************
  * @file    main.c
  * @author  Luis Gallet, Juan Carlos Borges
	* @version V1.2.0
  * @date    20-March-2017
  * @brief   Starts all used timers and threads
  ******************************************************************************
  */

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection
#include "Thread_7segment.h"								// Displays the temperature in the 4 digit 7 segment display
#include "Thread_adc.h"
#include "Thread_keypad.h"
#include "Thread_accelerometer.h"
#include "timer.h"

char LED_FLAG;
extern void initializeLED_IO			(void);
extern void start_Thread_LED			(void);
extern void Thread_LED(void const *argument);
extern osThreadId tid_Thread_LED;

TIM_HandleTypeDef handle_time3;
TIM_HandleTypeDef handle_tim4;

osMutexDef(uart_state_mutex);
osMutexId(uart_state_mutex_id);

/**
	These lines are mandatory to make CMSIS-RTOS RTX work with te new Cube HAL
*/
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) { 
  return os_time; 
}
#endif

/**
  * System Clock Configuration
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * Main function
  */
int main (void) {

  osKernelInitialize();                     /* initialize CMSIS-RTOS          */

  HAL_Init();                               /* Initialize the HAL Library     */

  SystemClock_Config();                     /* Configure the System Clock     */

	/* User codes goes here*/
	
	Init_TIM3_Config(&handle_time3);
	Init_TIM4_Config(&handle_tim4);
  //Start mutex				                      
	uart_state_mutex_id = osMutexCreate(osMutex(uart_state_mutex));
	//start_Thread_LED();                       
	start_Thread_keypad();
	start_Thread_adc();
	start_Thread_7segment();
	start_Thread_accelerometer();
	
	/* User codes ends here*/
  
	osKernelStart();                          /* start thread execution         */
	osDelay(osWaitForever);
}

void TIM3_IRQHandler(void){
	HAL_TIM_IRQHandler(&handle_time3);
}

void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0); 	
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *handle_tim){
	//Signal to Thread_ADC every 100Hz to get ADC values and process them
	osSignalSet ((osThreadId)getADCThreadId(), 0x0001);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//Signal to read values from ACC
	osSignalSet ((osThreadId)getACCThreadId(), 0x0002);
}