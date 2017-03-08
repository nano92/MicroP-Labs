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

GPIO_InitTypeDef GPIO_Col_Hash;
GPIO_InitTypeDef GPIO_Row_Hash;

GPIO_InitTypeDef GPIO_Acc;

static const uint16_t Row[4] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};
static const uint16_t Col[4] = {GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8};
char mapKeypad(int8_t column, int8_t row);

GPIO_InitTypeDef GPIOB_init;
GPIO_InitTypeDef GPIOA_init;
GPIO_InitTypeDef GPIOLED_init; 
uint8_t led = 0, temp_counter = 0, celsius = 0, rise_edge = 0;
uint8_t angle_index = 0;
/* Function: StartKeypadGPIO
 * Description: Initialises the GPIO pins responsible for the detection of the 4 by 4 keypad
 */
void InitReadButton(){
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_Row_Hash.Pin = GPIO_PIN_4;
	GPIO_Col_Hash.Pin = GPIO_PIN_8;
	
	GPIO_Row_Hash.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Col_Hash.Mode = GPIO_MODE_IT_FALLING;
	
	GPIO_Row_Hash.Pull = GPIO_NOPULL;
	GPIO_Col_Hash.Pull = GPIO_PULLUP;
	
	GPIO_Row_Hash.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Col_Hash.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	HAL_GPIO_Init(GPIOD, &GPIO_Row_Hash);
	HAL_GPIO_Init(GPIOD, &GPIO_Col_Hash);
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
}

void DeInitReadButton(){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_4);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8);
}

void InitAccGPIO(){
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_Acc.Pin = GPIO_PIN_0;
	GPIO_Acc.Mode = GPIO_MODE_IT_RISING;
	GPIO_Acc.Pull = GPIO_PULLDOWN;
	GPIO_Acc.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOE, &GPIO_Acc);
}
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
void DeInitKeypadGPIO(){
	for(uint8_t i = 0; i < 4; i++){
		HAL_GPIO_DeInit(GPIOD, Row[i]);
		HAL_GPIO_DeInit(GPIOD, Col[i]);
	}
}
uint8_t test_keypad(char angle[4]){
	int8_t col_index = -1;
	int8_t row_index = -1;
//	int8_t value = -1;
//	for(int i =0; i < 9000; i++){
//		HAL_GPIO_WritePin(GPIOD, Row[1], GPIO_PIN_RESET);
//		value = (HAL_GPIO_ReadPin(GPIOD, Col[1]) == GPIO_PIN_RESET)? 1 : 0;
//	}
//	printf("value = %d\n", value);
	while(col_index == -1) {
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOD, Row[i], GPIO_PIN_RESET);
		}
		for(int8_t i = 0; i < 4; i++){
				col_index = (HAL_GPIO_ReadPin(GPIOD, Col[i]) == GPIO_PIN_RESET)? i : -1;
				if (col_index >= 0) {						
					rise_edge = 1;
					break;
				}
		}
	}
	printf("rise: %d\n", rise_edge);
	if (rise_edge) {
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOD, Row[i], GPIO_PIN_SET);
		}

		for(int8_t j = 0; j < 4; j++){
			HAL_GPIO_WritePin(GPIOD, Row[j], GPIO_PIN_RESET);
			if(HAL_GPIO_ReadPin(GPIOD, Col[col_index]) == GPIO_PIN_RESET) {				
				row_index = j;
				break;
			}
			HAL_GPIO_WritePin(GPIOD, Row[j], GPIO_PIN_SET);
		}
	} 
	
	KeyBouncingDelay(GPIOD, Col[col_index], GPIO_PIN_RESET, rise_edge);
	
	if(col_index == 3 && row_index == 3){
		angle[angle_index + 1] = '\0'; 
		angle_index = 0;
		return 1;
	}
	
	if(col_index >= 0 && row_index >= 0){
		char button = mapKeypad(row_index, col_index);
		if(button == 'D'){
			if(angle_index > 0){
				angle_index--;
			}
			angle[angle_index] = '\0'; 
			printf("button = [%d\t%d] %c\n", col_index, row_index, button);
			return 0;
		}else if(angle_index < 3){
			angle[angle_index] = button;
			angle_index++;
		}
		printf("button = [%d\t%d] %c\n", col_index, row_index, button);
	}
	
	return 0;
}
/* Function : mapKeypad
	 Input    : int8_t column, int8_t row
	 Returns  : char value
   Description : maps the obtained column and row values to the appropiate character in the key pad
*/
char mapKeypad(int8_t column, int8_t row) {
	switch (column) {
		case 0: switch (row) {
			case 0: return '1'; break;
			case 1: return '4'; break;
			case 2: return '7'; break;
			case 3: return 'D'; break;// D for Delete
			default : return NULL;
		} break;
		case 1: switch (row) {
			case 0: return '2'; break;
			case 1: return '5'; break;
			case 2: return '8'; break;
			case 3: return '0'; break;
			default : return NULL;
		} break;
		case 3: switch (row) {
			case 0: return '3'; break;
			case 1: return '6'; break;
			case 2: return '9'; break;
			case 3: return 'E'; break;// E for Enter
			default : return NULL;
		}  break;
//		case 3: switch (row) {
//			case 0: return 'A'; break;
//			case 1: return 'B'; break;
//			case 2: return 'C'; break;
//			case 3: return '*'; break;// * since D is that the *-Key location
//			default : return NULL;
//		} break;
		default : return NULL;
	}		

}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> LAB 2 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
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
