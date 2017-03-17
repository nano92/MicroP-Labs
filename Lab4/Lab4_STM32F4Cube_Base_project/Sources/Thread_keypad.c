/*******************************************************************************
  * @file    Thread_keypad.c
  * @author  Juan Carlos Borges
	* @version V1.0.0
  * @date    16-March-2016
  * @brief   	
  ******************************************************************************
  */
#include "Thread_keypad.h"

extern const osMutexId uart_state_mutex_id;

void Thread_keypad (void const *argument); 
osThreadId tid_Thread_keypad;                         // thread id
osThreadDef(Thread_keypad, osPriorityNormal, 1, 0);

osMessageQId shared_KeyPadmsg_q_id;
osMessageQDef(keyPad_msg_queue, 16, char*);
osMessageQId (keyPad_queue_id);
osMessageQId shared_Statemsg_q_id;
osMessageQDef(state_msg_queue, 16, char*);
osMessageQId (state_queue_id);

static void InitReadButton(void);
static void InitAccGPIO(void);
static void DeInitReadButton(void);
static void StartKeypadGPIO(void);
static void DeInitKeypadGPIO(void);
static uint8_t read_keypad(int8_t coord[2]);
static void StartLEDGPIO(void);
static void setKeyPadMsgQueueId(osMessageQId msg_Id);
static void setStateMsgQueueId(osMessageQId msg_Id);
uint32_t setState (uint8_t pos, uint32_t state);
void resetParameters (int8_t counter, uint8_t command_index, char command[4][9]);

GPIO_InitTypeDef GPIO_Row_init;
GPIO_InitTypeDef GPIO_Col_init;

GPIO_InitTypeDef GPIO_Col_Hash;
GPIO_InitTypeDef GPIO_Row_Hash;

static const uint16_t Row[4] = {GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14};
static const uint16_t Col[4] = {GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12};

static char mapKeypad(int8_t column, int8_t row);

static uint8_t rise_edge = 0;
static uint8_t angle_index = 0;

char ANGLE_FLAG = 0;
char reading = 0;

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
void Thread_keypad (void const *argument) {
	StartKeypadGPIO();
	
	int16_t pitch_angle = 0, roll_angle = 0;
	
	uint8_t stat = 0;
	char angle[2][4];
	memset(angle, '\0', sizeof(angle[0][0]) * 2 * 4);
	
	char command[4][9];
	for(uint8_t i = 0; i < 4; i++){
		Decoding(-1, command[i]);
	}
	
	char temp_command[9];
	int8_t coord[2] = {-1,-1};
	int8_t counter = -1;
	int8_t number = -1;
	uint8_t command_index = 0;
	
	keyPad_queue_id = osMessageCreate(osMessageQ(keyPad_msg_queue), NULL);
	setKeyPadMsgQueueId(keyPad_queue_id);
	
	state_queue_id = osMessageCreate(osMessageQ(state_msg_queue), NULL);
	setStateMsgQueueId(state_queue_id);
	
	uint8_t state = 1;
	while(1){
		osMessagePut(state_queue_id, (uint32_t)state, osWaitForever);
		osMessagePut(keyPad_queue_id, (uint32_t)(command), osWaitForever);
		stat = read_keypad(coord);
		printf("coord = [col_%d, row_%d]\n", coord[0], coord[1]);
		if(coord[0] != -1){
			switch(stat) {
				case 0 : if(reading) {
						char button = mapKeypad(coord[0], coord[1]);
						if(button == 'D'){ // Delete: Key *
								if(angle_index > 0){
									angle_index--;
									angle[counter][angle_index] = '\0';
								}
						} else if((angle_index < 3) && (button != NULL)){ // Keys: 0 - 9
							angle[counter][angle_index] = button;				
							angle_index++;
							number = button - '0';
							Decoding(number, temp_command);
							strcpy(command[command_index], temp_command);
							command_index++;
						} else { // Keys A to D
							state = setState(coord[1],state);
						}
					} else { // When not inputting values
						if (coord[0] == 3) {
							state = setState(coord[1],state);
						}
				}; break;
				case 1 : if(counter == -1){// Start inputting pitch angle
					osMutexWait(uart_state_mutex_id, 10);
					reading = 1;
					state = 2; // read keypad
					counter++;
				} else if (counter == 1){ // Finish inputting roll angle
					roll_angle = atoi(angle[counter]);
					printf("roll angle = %d\n", roll_angle);
					setRoll(roll_angle);
					resetParameters(counter, command_index, command);
					state = 1;
					reading = 0;
					osMutexRelease(uart_state_mutex_id);
					//Send acc
				} else { // Finish inputting pitch angle, start inputting roll angle
					pitch_angle = atoi(angle[counter]);
					printf("pitch angle = %d\n", pitch_angle);
					setPitch(pitch_angle);
					angle_index = 0;
					counter ++;
				}; break;
				case 2 : {// Reset everyting
					resetParameters(counter, command_index, command);
					memset(angle, '\0', sizeof(angle[0][0]) * 2 * 4);
				}; break;			
				default : printf("Angle[%d] = %s\n", counter, angle[counter]); 
			}
//			if (coord[0] == 3) {
//				state = setState(coord[1],state);
//			}
			coord[0] = -1;
			coord[1] = -1;
			if(counter > -1)
				printf("Angle[%d] = %s\n", counter, angle[counter]);
		}
	}
}

void resetParameters (int8_t counter, uint8_t command_index, char command[4][9]){
			counter = -1;
			angle_index = 0;
			command_index = 0;
			for(uint8_t i = 0; i < 4; i++){
						Decoding(-1, command[i]);
			}
}

uint32_t setState (uint8_t pos, uint32_t state) {
		switch(pos) {
				case 0 : return 1; break; // Display temperature
				case 1 : return 2; break; // Display Keypad
				case 2 : return 3; break; // Display accelerometer roll
				case 3 : return 4; break; // Display accelerometer pitch
				default : return state;	 // When no key is pressed, continue displaying previous state						
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

/* Function: StartKeypadGPIO
 * Description: Initialises the GPIO pins responsible for the detection of the 
 * 4 by 4 keypad
 */

void StartKeypadGPIO(void){

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_Row_init.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_Col_init.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;

	GPIO_Row_init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Col_init.Mode = GPIO_MODE_INPUT;
		
	GPIO_Row_init.Pull = GPIO_NOPULL;
	GPIO_Col_init.Pull = GPIO_PULLUP;

	GPIO_Row_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Col_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	HAL_GPIO_Init(GPIOB, &GPIO_Row_init);
	HAL_GPIO_Init(GPIOE, &GPIO_Col_init);
	
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
uint8_t read_keypad(int8_t coord[2]){
	int8_t col_index = -1;
	int8_t row_index = -1;
	uint8_t stat = 0;
	uint16_t counter = 0;

	while(col_index == -1) {
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOB, Row[i], GPIO_PIN_RESET);
		}
		for(int8_t i = 0; i < 4; i++){
				col_index = (HAL_GPIO_ReadPin(GPIOE, Col[i]) == GPIO_PIN_RESET)? i : -1;
				if (col_index >= 0) {						
					rise_edge = 1;
					break;
				}
		}
		if(counter == 500 && !reading) {
			return stat;
		}
		counter ++;
	}
	
	int16_t button_counter = 0;
	while(row_index == -1){
		for(int8_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(GPIOB, Row[i], GPIO_PIN_SET);
		}

		for(int8_t j = 0; j < 4; j++){
			HAL_GPIO_WritePin(GPIOB, Row[j], GPIO_PIN_RESET);
			if(HAL_GPIO_ReadPin(GPIOE, Col[col_index]) == GPIO_PIN_RESET) {				
				row_index = j;
				break;
			}
			HAL_GPIO_WritePin(GPIOB, Row[j], GPIO_PIN_SET);
		}
	}
		while(rise_edge) {
		rise_edge = (HAL_GPIO_ReadPin(GPIOE, Col[col_index]) == GPIO_PIN_RESET);
		if ((col_index == 0) && (row_index == 3)) {
			button_counter++;
		}
	}
	
	if (button_counter > 500) {
		stat = 2;
	} else {
		stat = 0;
	}
	
	coord[0] = col_index;
	coord[1] = row_index;
	//KeyBouncingDelay(GPIOD, Col[col_index], GPIO_PIN_RESET, coord, rise_edge, stat);
	
	if(col_index == 2 && row_index == 3){
		return 1;
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
void setKeyPadMsgQueueId(osMessageQId msg_Id){
	shared_KeyPadmsg_q_id = msg_Id;
}

osMessageQId getKeyPadMsgQueueId(void){
	return shared_KeyPadmsg_q_id;
}
void setStateMsgQueueId(osMessageQId msg_Id){
	shared_Statemsg_q_id = msg_Id;
}

osMessageQId getStateMsgQueueId(void){
	return shared_Statemsg_q_id;
}