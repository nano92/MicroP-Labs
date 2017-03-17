/*******************************************************************************
  * @file    Thread_7segment.c
  * @author  Luis Gallet
	* @version V1.0.0
  * @date    15-March-2016
  * @brief   	
  ******************************************************************************
  */
	
#include "Thread_7segment.h"
#include "Thread_adc.h"
#include "Thread_keypad.h"
#include "Thread_accelerometer.h"

osMutexId osMutexCreate (const osMutexDef_t *mutex_def);

void Thread_7segment (void const *argument);             // thread function
osThreadId tid_Thread_7segement;                         // thread id
osThreadDef(Thread_7segment, osPriorityNormal, 1, 0);

static void DisplayTemperature(char command[4][9]);
static void turnOffDisplay(void);
GPIO_InitTypeDef GPIOD_init;
GPIO_InitTypeDef GPIOB_init;
GPIO_InitTypeDef GPIOA_init;

//char command[4][9] = {"11000000","11000000","11000000","11000000"};

// GPIOs for the 7 segments
static const uint16_t GPIOD_array[8] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, 
															GPIO_PIN_4, GPIO_PIN_5 , GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8};
// GPIOs for the digit
static const uint16_t GPIOB_array[4] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};															
static uint8_t temp_counter = 0, celsius = 0, rise_edge = 0;

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_7segment (void) {

  tid_Thread_7segement = osThreadCreate(osThread(Thread_7segment ), NULL); // Start LED_Thread
	if (!tid_Thread_7segement) return(-1); 
  return(0);
}

/*----------------------------------------------------------------------------
 *      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
void Thread_7segment (void const *argument) {
	 Start7SegmentDisplayGPIO();
	char command[4][9];
	memset(command, 0, sizeof(command[0][0]) * 9 * 4);
	osEvent event_adc, event_alarm, event_keypad, event_acc_roll, event_acc_pitch, event_state; //event_state : comes from keypad
	uint8_t alarm = 1;
	uint16_t count = 0;
	uint16_t time_delay = 100;
	uint8_t state = 0;
	while(1){
		uint16_t delay = 0;
		
		event_state = osMessageGet((osMessageQId)getStateMsgQueueId(), osWaitForever);
		event_adc = osMessageGet((osMessageQId)getADCMsgQueueId(), osWaitForever);
		event_alarm = osMessageGet((osMessageQId)getAlarmMsgQueueId(), osWaitForever);
		event_keypad = osMessageGet((osMessageQId)getKeyPadMsgQueueId(), osWaitForever);
		//event_acc_roll = osMessageGet((osMessageQId)getRollACCMsgQueueId(), osWaitForever);
		//event_acc_pitch = osMessageGet((osMessageQId)getPitchACCMsgQueueId(), osWaitForever);
		
		if(event_state.status == osEventMessage){
			state = (uint8_t)event_state.value.p;
		}
		
		if(event_alarm.status == osEventMessage){
			alarm = (uint8_t)event_alarm.value.p;
		}
		
		switch (state) {
			case 2 : if(event_keypad.status == osEventMessage){		
				for(uint8_t i = 0; i < 4; i++){
					strcpy(command[i],(char *)event_keypad.value.p + i*9);
				}
			};break;
			case 3 : if(event_acc_roll.status == osEventMessage){		
				for(uint8_t i = 0; i < 4; i++){
					strcpy(command[i],(char *)event_acc_roll.value.p + i*9);
				}
			};break;
			case 4 : if(event_acc_pitch.status == osEventMessage){		
				for(uint8_t i = 0; i < 4; i++){
					strcpy(command[i],(char *)event_acc_pitch.value.p + i*9);
				}
			};break;
			default: if(event_adc.status == osEventMessage){		
				for(uint8_t i = 0; i < 4; i++){
					strcpy(command[i],(char *)event_adc.value.p + i*9);
				}
			};
		}

		while(delay < time_delay){
			if(alarm == 2 ){
				if (count == 1) {
					turnOffDisplay();
				} else if (count == 2) {
					count = -1;
					DisplayTemperature(command);
				} else {
					DisplayTemperature(command);
				}
			} else {
				DisplayTemperature(command);
			}
			delay++;
		}
		count = (alarm == 2) ? count + 1 : 0;
	}	
}

/* Function: Start7SegmentDisplayGPIO
 * Description: Initialises the GPIO pins responsible for the 4 digits 7 segment display
 */
void Start7SegmentDisplayGPIO(void){
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	
	GPIOD_init.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
										| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 ;
	GPIOB_init.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
		
	GPIOD_init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOB_init.Mode = GPIO_MODE_OUTPUT_PP;
		
	GPIOD_init.Pull = GPIO_PULLDOWN;
	GPIOB_init.Pull = GPIO_PULLDOWN;
		
	GPIOD_init.Speed = GPIO_SPEED_FREQ_LOW ;
	GPIOB_init.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOD, &GPIOD_init);
	HAL_GPIO_Init(GPIOB, &GPIOB_init);
}

/* Function: DisplayTemperature
 * Input   : char command[4][9], char temp_alarm
 * Description: function in charge of displaying the commands through the 4 digit 7 segment display and displaying the LED alarm signal 
 * ones temp_alarm is high.
 */
void DisplayTemperature(char command[4][9]){	
	// Displaying the command in 7 segment display
	for(int8_t d = 0; d < 4; d++){
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[d], GPIO_PIN_RESET);
	}
		
	for(int8_t n = 0; n < 4; n++){
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[n], GPIO_PIN_SET);
		
		for(uint16_t i = 1400; i > 0; i--){
			for(int8_t j = 7; j >= 0; j--){
				if(command[n][7 - j] == '1'){
					//printf("%c", command[n][i]);
					HAL_GPIO_WritePin(GPIOD, GPIOD_array[j], GPIO_PIN_RESET);
				}else{
					//printf("%c", command[n][i]);
					HAL_GPIO_WritePin(GPIOD, GPIOD_array[j], GPIO_PIN_SET);
				}
			}
		}	
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[n], GPIO_PIN_RESET);		//printf("\n");
		/*if(temp_alarm){
			//turnOffDisplay();
		}*/
	}
}

void turnOffDisplay(void) {
	
	for(int8_t i = 0; i < 4; i++){
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[i], GPIO_PIN_RESET);
	}
	for(int8_t n = 0; n < 4; n++) {
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[n], GPIO_PIN_SET);
		for(uint16_t i = 1400; i > 0; i--){
			for(int8_t i = 7; i >= 0; i--){
					HAL_GPIO_WritePin(GPIOD, GPIOD_array[i], GPIO_PIN_RESET);
			}
		}	
		HAL_GPIO_WritePin(GPIOB, GPIOB_array[n], GPIO_PIN_RESET);
	}
}

osThreadId get7SegementThreadId(void){
	return tid_Thread_7segement;
}