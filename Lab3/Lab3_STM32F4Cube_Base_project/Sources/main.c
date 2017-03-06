/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program subroutine
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "lis3dsh.h"

/* Private variables ---------------------------------------------------------*/
LIS3DSH_InitTypeDef LIS3DSH_InitStruct;
LIS3DSH_DRYInterruptConfigTypeDef LIS3DSH_IntConfigStruct;
static const uint16_t Col[4] = {GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9};
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);

int main(void)
{	
  /* MCU Configuration----------------------------------------------------------*/

  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
	
	//HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 3);
	HAL_NVIC_SetPriorityGrouping(2);
	//HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 3);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	LIS3DSH_Init(&LIS3DSH_InitStruct);
	LIS3DSH_DataRateCmd(LIS3DSH_DATARATE_25);
	LIS3DSH_InterruptConfigStruct(&LIS3DSH_IntConfigStruct);
	LIS3DSH_DataReadyInterruptConfig(&LIS3DSH_IntConfigStruct);
	
	
	while(1){
		InitReadButton();
		if(INPUT_FLAG){
			INPUT_FLAG = 0;
			DeInitReadButton();
			StartKeypadGPIO();
			uint8_t end = test_keypad();
			while(!end){
				end = test_keypad();
			}
			DeInitKeypadGPIO();
		}
//		if(HAL_GPIO_ReadPin(GPIOD, Col[0]) == 1 ||
//				HAL_GPIO_ReadPin(GPIOD, Col[1]) == 1 ||
//				HAL_GPIO_ReadPin(GPIOD, Col[2]) == 1 ||
//				HAL_GPIO_ReadPin(GPIOD, Col[3]) == 1){
//				 
//			HAL_NVIC_SetPendingIRQ(EXTI9_5_IRQn);
//		}
	}
}

/** System Clock Configuration*/
void SystemClock_Config(void){

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType 	= RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState 			 	= RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM 				= 8;
  RCC_OscInitStruct.PLL.PLLN 				= 336;
  RCC_OscInitStruct.PLL.PLLP 				= RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ 				= 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};

  RCC_ClkInitStruct.ClockType 			= RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5)!= HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};
	
	/*Configures SysTick to provide 1ms interval interrupts. SysTick is already 
	  configured inside HAL_Init, I don't kow why the CubeMX generates this call again*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* This function sets the source clock for the internal SysTick Timer to be the maximum,
	   in our case, HCLK is now 168MHz*/
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line){
}
#endif

