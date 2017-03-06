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
// Assigning general variables for the GPIO fucntions
GPIO_InitTypeDef GPIO_Row_init;
GPIO_InitTypeDef GPIO_Col_init;

GPIO_InitTypeDef GPIOB_init;
GPIO_InitTypeDef GPIOA_init;
GPIO_InitTypeDef GPIOLED_init;

uint8_t led = 0, temp_counter = 0, celsius = 0, rise_edge = 0;

/* Function: Start7SegmentDisplayGPIO
 * Description: Initialises the GPIO pins responsible for the 4 digits 7 segment display
 */
void StartKeypadGPIO(){
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	
	
	GPIO_Row_init.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
	GPIO_Col_init.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
	
	//GPIOB_init.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
		
	GPIO_Row_init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Col_init.Mode = GPIO_MODE_INPUT;
		
	GPIO_Row_init.Pull = GPIO_NOPULL;
	GPIO_Col_init.Pull = GPIO_PULLUP;
		
	//GPIOD_init.Speed = GPIO_SPEED_FREQ_LOW ;
	//GPIOB_init.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_Row_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Col_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	HAL_GPIO_Init(GPIOD, &GPIO_Row_init);
	HAL_GPIO_Init(GPIOD, &GPIO_Col_init);
	
}

void test_keypad(){
	const uint16_t Row[4] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};
	const uint16_t Col[4] = {GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8};
	int8_t col_value[4] = {0, 0, 0, 0};
	int8_t col_index = -1;
	
	for(int16_t d = 9000; d > 0; d--) {
		int8_t col_index = -1;	
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOD, Row[i], GPIO_PIN_RESET);
		}
		for(int8_t i = 0; i < 4; i++){
				col_value[i] = HAL_GPIO_ReadPin(GPIOD, Col[i]);
	}
	if (col_value[0] || col_value[1] || col_value[2] || col_value[3]) {
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOD, Row[i], GPIO_PIN_SET);
		}
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOD, Row[i], GPIO_PIN_RESET);

			for(int8_t i = 0; i < 4; i++){
				int8_t value = HAL_GPIO_ReadPin(GPIOD, Col[i]);
				col_index = (value == 0) ? i : -1;
				if(col_index != -1){
					break;
				}	
			}
			if(col_index != -1){
					break;
			}
			HAL_GPIO_WritePin(GPIOD, Row[i], GPIO_PIN_SET);
		}
	} 
	if(col_index >= 0){
		printf("col_index = %d\n", col_index);
	}
		
	
	
}
/* Function: StartButtonGPIO
 * Description: Initialises the GPIO pin responsible for the user button that would change the temperature measurement from 
 * Celcius to Farenheit.
 */
void StartButtonGPIO(){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIOA_init.Pin = GPIO_PIN_0;
	GPIOA_init.Mode = GPIO_MODE_IT_FALLING;
	GPIOA_init.Pull = GPIO_NOPULL;
	
	HAL_GPIO_Init(GPIOA, &GPIOA_init);
}

/* Function: StartLEDGPIO
 * Description: Initialises the GPIO pins responsible for the 4 LED used for the temperature alarm
 */
void StartLEDGPIO(){
	GPIOLED_init.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIOLED_init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOLED_init.Pull = GPIO_PULLDOWN;
	GPIOLED_init.Speed = GPIO_SPEED_FAST;
	
	HAL_GPIO_Init(GPIOD, &GPIOLED_init);
}

/* Function: changeDisplay
 * Returns : uint8_t celcius
 * Description: Changes the flag for displaying either celcius(0) or farenheit(1) when the button is pressed
 */
uint8_t changeDisplay(){
	uint8_t value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	if(value ^ rise_edge){
		celsius = (value) ? !celsius : celsius;
	}
	rise_edge = value;
	return celsius;
}

/* Function: DisplayTemperature
 * Input   : char command[4][9], char temp_alarm
 * Description: function in charge of displaying the commands through the 4 digit 7 segment display and displaying the LED alarm signal 
 * ones temp_alarm is high.
 */
void DisplayTemperature(char command[4][9], char temp_alarm){
	
	const uint16_t GPIOD_array[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, 
															GPIO_PIN_4, GPIO_PIN_5 , GPIO_PIN_6, GPIO_PIN_7};
	
	const uint16_t LED_array[4] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};
	
	const uint16_t GPIOB_array[4] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
	
	// Displaying the command in 7 segment display
	for(int8_t i = 0; i < 4; i++){
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[i], GPIO_PIN_SET);
	}
		
	for(int8_t n = 0; n < 4; n++) {
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[n], GPIO_PIN_RESET);
		
		for(uint16_t i = 1400; i > 0; i--){
			for(int8_t i = 7; i >= 0; i--){
				if(command[n][7 - i] == '1'){
					//printf("%c", command[n][i]);
					HAL_GPIO_WritePin(GPIOD, GPIOD_array[i], GPIO_PIN_RESET);
				}else{
					//printf("%c", command[n][i]);
					HAL_GPIO_WritePin(GPIOD, GPIOD_array[i], GPIO_PIN_SET);
				}
			}
		}	
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[n], GPIO_PIN_SET);		//printf("\n");	
	}
	
	// Takes care of dislpaying the alarm
	if(temp_alarm){
		HAL_GPIO_TogglePin(GPIOD, LED_array[led]);
		if (temp_counter == 16) {
				temp_counter = 0;
				led = (led == 4) ? 0 : led+1;
		} else {
				temp_counter += 1;
		}
	}else{
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOD, LED_array[i], GPIO_PIN_RESET);
		}
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
