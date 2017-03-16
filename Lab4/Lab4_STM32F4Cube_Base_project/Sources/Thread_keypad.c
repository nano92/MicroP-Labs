/*******************************************************************************
  * @file    Thread_keypad.c
  * @author  Juan Carlos Borges
	* @version V1.0.0
  * @date    16-March-2016
  * @brief   	
  ******************************************************************************
  */
#include "keypadGpio.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"

void Thread_keypad (void const *argument); 
osThreadId tid_Thread_keypad;                         // thread id
osThreadDef(Thread_keypad, osPriorityNormal, 1, 0);

GPIO_InitTypeDef GPIO_Row_init;
GPIO_InitTypeDef GPIO_Col_init;

GPIO_InitTypeDef GPIO_Col_Hash;
GPIO_InitTypeDef GPIO_Row_Hash;

GPIO_InitTypeDef GPIO_Acc;
GPIO_InitTypeDef GPIOLED_init; 

static const uint16_t Row[4] = {GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14};
static const uint16_t Col[4] = {GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12};

static char mapKeypad(int8_t column, int8_t row);

static uint8_t rise_edge = 0;
static uint8_t angle_index = 0;

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_keypad (void) {

  tid_Thread_keypad = osThreadCreate(osThread(Thread_keypad), NULL); // Start LED_Thread
  if (!tid_Thread_keypad) return(-1); 
  return(0);
}
/*----------------------------------------------------------------------------
 *      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
void Thread_7segment (void const *argument) {
	while(1){
		
	}
}

/* Function: StartKeypadGPIO
 * Description: Initialises the GPIO pins responsible for the detection of the 
 * 4 by 4 keypad
 */
void InitReadButton(void){
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_Row_Hash.Pin = GPIO_PIN_13;
	GPIO_Col_Hash.Pin = GPIO_PIN_12;
	
	GPIO_Row_Hash.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Col_Hash.Mode = GPIO_MODE_IT_FALLING;
	
	GPIO_Row_Hash.Pull = GPIO_NOPULL;
	GPIO_Col_Hash.Pull = GPIO_PULLUP;
	
	GPIO_Row_Hash.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Col_Hash.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	HAL_GPIO_Init(GPIOB, &GPIO_Row_Hash);
	HAL_GPIO_Init(GPIOD, &GPIO_Col_Hash);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}

/* Function: DeInitReadButton()
* Description: Initialises the gpio pins for the interrupt button of the keypad.
*/
void DeInitReadButton(void){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_13);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12);
}

/* Function: InitAccGPIO()
* Description: Initialises the accelerometer gpio pins
*/
void InitAccGPIO(void){
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_Acc.Pin = GPIO_PIN_0;
	GPIO_Acc.Mode = GPIO_MODE_IT_RISING;
	GPIO_Acc.Pull = GPIO_PULLDOWN;
	GPIO_Acc.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOE, &GPIO_Acc);
}

/* Function: StartKeypadGPIO
 * Description: Initialises the GPIO pins responsible for the detection of the 
 * 4 by 4 keypad
 */

void StartKeypadGPIO(void){

	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_Row_init.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_Col_init.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;

	GPIO_Row_init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Col_init.Mode = GPIO_MODE_INPUT;
		
	GPIO_Row_init.Pull = GPIO_NOPULL;
	GPIO_Col_init.Pull = GPIO_PULLUP;

	GPIO_Row_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Col_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	HAL_GPIO_Init(GPIOB, &GPIO_Row_init);
	HAL_GPIO_Init(GPIOD, &GPIO_Col_init);
	
}

/* Function: DeInitKeypadGPIO()
* Description: Initialises the GPIOs for the rows and columns of the matrix
*/
void DeInitKeypadGPIO(void){
	for(uint8_t i = 0; i < 4; i++){
		HAL_GPIO_DeInit(GPIOB, Row[i]);
		HAL_GPIO_DeInit(GPIOD, Col[i]);
	}
}

/* Function : read_keypad()
* Input : char angle[4];
* Returns: uint8_t;
* Definition: Reads and decodes the keypad pressing. Returns a 1 when the # key 
* is pressed
*/
uint8_t read_keypad(char angle[4]){
	int8_t col_index = -1;
	int8_t row_index = -1;
	uint8_t stat = 0;

	while(col_index == -1) {
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOB, Row[i], GPIO_PIN_RESET);
		}
		for(int8_t i = 0; i < 4; i++){
				col_index = (HAL_GPIO_ReadPin(GPIOD, Col[i]) == GPIO_PIN_RESET)? i : -1;
				if (col_index >= 0) {						
					rise_edge = 1;
					break;
				}
		}
	}

	if (rise_edge) {
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOB, Row[i], GPIO_PIN_SET);
		}

		for(int8_t j = 0; j < 4; j++){
			HAL_GPIO_WritePin(GPIOB, Row[j], GPIO_PIN_RESET);
			if(HAL_GPIO_ReadPin(GPIOD, Col[col_index]) == GPIO_PIN_RESET) {				
				row_index = j;
				break;
			}
			HAL_GPIO_WritePin(GPIOD, Row[j], GPIO_PIN_SET);
		}
	} 
	
	uint8_t coord[2] = {col_index, row_index};
	KeyBouncingDelay(GPIOD, Col[col_index], GPIO_PIN_RESET, coord, rise_edge, stat);
	
	if(col_index == 3 && row_index == 2){
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
			return stat;
		}else if(angle_index < 3){
			angle[angle_index] = button;
			angle_index++;
		}
		printf("button = [%d\t%d] %c\n", col_index, row_index, button);
	}
	
	return stat;
}
/* Function : mapKeypad
 * Input    : int8_t column, int8_t row
 * Returns  : char value
 * Description : maps the obtained column and row values to the appropiate 
 * character in the key pad
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
		case 2: switch (row) {
			case 0: return '3'; break;
			case 1: return '6'; break;
			case 2: return '9'; break;
			case 3: return 'E'; break;// E for Enter
			default : return NULL;
		}  break;
		default : return NULL;
	}		

}
/* Function: StartLEDGPIO
 * Description: Initialises the GPIO pins responsible for the 4 LED used for the temperature alarm
 */
void StartLEDGPIO(void){
	GPIOLED_init.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIOLED_init.Mode = GPIO_MODE_AF_PP;
	GPIOLED_init.Pull = GPIO_PULLDOWN;
	GPIOLED_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIOLED_init.Alternate = GPIO_AF2_TIM4;
	
	HAL_GPIO_Init(GPIOD, &GPIOLED_init);
}