/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "gpio.h"
#include "stm32f4xx_it.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

ADC_HandleTypeDef ADC1_Handle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void CommandGenerator(uint32_t ADC_value, char command[4][9]);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	HAL_StatusTypeDef status;
	uint32_t ADC_value = 0;
	char command[4][9]; 
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN 2 */

	status = StartADCHandle(&ADC1_Handle);
	if(status != HAL_OK){
		printf("StartADCHandle status: %d\n", status);
		//Need to add an interrupt handler
	}
	
	Start7SegmentDisplayGPIO();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//printf("flag: %d\n", flag);
		if(flag){
			flag = 0;
			//printf("flag: %d\n", flag);
			//DisplayTemperature();
			//testButton();
			status = GetTempValue(&ADC1_Handle, &ADC_value);
			if(status == HAL_OK){
				CommandGenerator(ADC_value, command);
				DisplayTemperature(command);
				//printf("Success\n");
			}else{
				//printf("Error\n");
			}
		}
		
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/10);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
int16_t DegreeConverter(char temp_flag, uint32_t ADC_value){

	// Because the function described decimal numbers that could only be
	// represented in the floating point format, the equation was translated so
	// that the calculation can be done by using integers while arriving to the
	// answer.
	int16_t celsius = (((50*ADC_value - 38)/125)/1000) + 25;
	int16_t farenheit = (celsius * 9)/5 + 32;
	
	return (temp_flag == 0) ? celsius : farenheit;
}

void Decoding(int8_t number, char segment[9]) {
	switch (number) {
			case 0 : strcpy(segment, "11000000\0"); break;
			case 1 : strcpy(segment, "11111001\0"); break;
			case 2 : strcpy(segment, "10100100\0"); break;
			case 3 : strcpy(segment, "10110000\0"); break;
			case 4 : strcpy(segment, "10011001\0"); break;
			case 5 : strcpy(segment, "10010010\0"); break;
			case 6 : strcpy(segment, "10000010\0"); break;
			case 7 : strcpy(segment, "11111000\0"); break;
			case 8 : strcpy(segment, "10000000\0"); break;
			case 9 : strcpy(segment, "10011000\0"); break;
			case 10 : strcpy(segment,"10111111\0"); break;
			default  : strcpy(segment,"11111111\0");// Off
	}
	
}

void CommandGenerator(uint32_t ADC_value, char command[4][9]){
	int16_t res = 0;
	memset(command, 0, sizeof(command[0][0]) * 9 * 4);
	
	int16_t number = DegreeConverter(0, ADC_value);
	int8_t dec[4] = {-1, -1, -1, 0};
	
	if(number < 0){
		dec[0] = 10;
		res = -1 * number;
	}else{
		res = number;
	}
	uint8_t i = 3;
	
	while(res > 10){
		dec[i] = res % 10;
		res = res / 10;
		if(res < 10){
			dec[i - 1] = res;
		}else{
			i--;
		}
	}
	char cmd[9];
		
	for(int8_t i = 0; i < 4; i++) {
		//memset(cmd, 0, 8);
		Decoding(dec[i],cmd);
		strcpy(command[i],cmd);
		printf("cmd = %s\n", cmd);
	}
	printf("command[%d] = %s\n", 0, command[0]);
}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
