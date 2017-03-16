/*******************************************************************************
  * @file    Thread_adc.c
  * @author  Luis Gallet
	* @version V1.0.0
  * @date    15-March-2016
  * @brief   	
  ******************************************************************************
  */

#include "Thread_adc.h"
#include "supporting_functions.h"

void Thread_adc(void const *argument);             // thread function
osThreadId tid_Thread_adc;                         // thread id
osThreadDef(Thread_adc, osPriorityNormal, 1, 0);

osMessageQId shared_ADCmsg_q_id, shared_alarm_msg_q_id;
osMessageQDef(adc_msg_queue, 16, char*);
osMessageQId (adc_msg_queue_id);
osMessageQDef(alarm_msg_queue, 16, char*);
osMessageQId (alarm_msg_queue_id);


static void setADCMsgQueueId(osMessageQId msg_Id);
static HAL_StatusTypeDef StartADCHandle(ADC_HandleTypeDef *ADC1_Handle);
static HAL_StatusTypeDef GetTempValue(ADC_HandleTypeDef* ADC1_Handle, uint32_t* ADC_value);
static uint32_t filter(uint32_t data[5]);
static float DegreeConverter(uint32_t ADC_value);
static void CommandGenerator(uint32_t ADC_value, char command[4][9]);
static void setTempAlarm(uint8_t alarm);
static uint8_t getTempAlarm(void);
static void setAlarmMsgQueueId(osMessageQId msg_Id);
static void setADCMsgQueueId(osMessageQId msg_Id);

ADC_HandleTypeDef ADC1_Handle;
ADC_InitTypeDef ADC1_init;
ADC_ChannelConfTypeDef channel_config;
ADC_MultiModeTypeDef multi_mode_config;

uint8_t temp_alarm = 1;
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
	HAL_StatusTypeDef status;
	StartADCHandle(&ADC1_Handle);
	uint32_t ADC_value = 0;
	uint32_t data[5] = {0,0,0,0,0};
	char command[4][9];
	adc_msg_queue_id = osMessageCreate(osMessageQ(adc_msg_queue), NULL);
	setADCMsgQueueId(adc_msg_queue_id);
	alarm_msg_queue_id = osMessageCreate(osMessageQ(alarm_msg_queue), NULL);
	setAlarmMsgQueueId(alarm_msg_queue_id);
	while(1){
		osDelay(1000);
		status = GetTempValue(&ADC1_Handle, &ADC_value);
		data[0] = ADC_value;
		ADC_value = filter(data);
		if(status == HAL_OK){
			CommandGenerator(ADC_value, command);
			//printf("command from adc = %s %s %s %s\n", command[0],command[1], command[2], command[3]);
			osMessagePut(adc_msg_queue_id, (uint32_t)(command), osWaitForever);
			osMessagePut(alarm_msg_queue_id, (uint32_t)getTempAlarm(), osWaitForever);
		}
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

/* Function : CommandGenerator
 * Input    : uint32_t ADC_value
 * Output   : char command[4][9]
 * Description: Generates the 4 digit display's segment commands.
 */
void CommandGenerator(uint32_t ADC_value, char command[4][9]){
	int16_t res = 0;
	memset(command, 0, sizeof(command[0][0]) * 9 * 4);
	
	float number = DegreeConverter(ADC_value);
	int8_t dec[4] = {-1, -1, -1, 0};
	
	//Need to show only one decimal, then temperature value is multiplied by 10 
	//and cast to be an integer
	number = number * 10;
	
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
		if(i == 2){
			cmd[0] = '0';
		}
		strcpy(command[i],cmd);
	}
	
}

/* Fucntion : DegreeConverter
 * Input    : uint32_t ADC_value
 * Returns  : float
 * Description: Function takes in the interger obtained from the ADC sampling and transforms it into a temperature value in 
 * the form of a float. Depending on current value of the variable temp_flag, the returned temperature value will be in either
 * Celsius (if temp_flag = 0) or in Farenheit (if temp_flag = 0).
 */
float DegreeConverter(uint32_t ADC_value){
	//uint8_t temp_flag = changeDisplay();
	
	float celsius = (((50.0*ADC_value - 38.0)/125.0)/1000.0) + 25.0;
	//float farenheit = (celsius * 9.0)/5.0 + 32.0;
	
	//Set high temperature alarm after it gets calculated. Global variable
	//temp_alarm is used in DisplayTemperature()
	uint8_t alarm = (celsius > 31.0) ? 2 : 1;
	setTempAlarm(alarm);

	//return (temp_flag == 0) ? celsius : farenheit;
	return celsius;
}

/* Function : filter
 * Input    : uint32_t data[5]
 * Returns  : uint32_t
 * Description : Applies a FIR filter accros the data obtained in the ADC.
 */
uint32_t filter(uint32_t data[5]){
	uint32_t filter = (10*(data[0]+data[4])+15*(data[1]+data[3])+ 50*data[2])/100;
	for(uint8_t i = 4; i >= 1; i--) {
			data[i] = data[i-1];
	}
	return filter;
}

void setTempAlarm(uint8_t alarm){
	temp_alarm = alarm;
}

uint8_t getTempAlarm(void){
	return temp_alarm;
}

void setAlarmMsgQueueId(osMessageQId msg_Id){
	shared_alarm_msg_q_id = msg_Id;
}

osMessageQId getAlarmMsgQueueId(void){
	return shared_alarm_msg_q_id;
}

void setADCMsgQueueId(osMessageQId msg_Id){
	shared_ADCmsg_q_id = msg_Id;
}

osMessageQId getADCMsgQueueId(void){
	return shared_ADCmsg_q_id;
}