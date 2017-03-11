/**
  ******************************************************************************
  * File Name          : supporting_functions.c
  * Description        : User defined common functions that can be used across projects
	* Author						 : Juan Carlos Borges, Luis Gallet
  * Group              : 10	
	* Version            : 2.0.0
	* Date							 : March 10th, 2017
  ******************************************************************************
  */
#include "supporting_functions.h"
/**
   * @brief A function used to relay information (PRINTF) that a specific error has occured 
	 * and gets the system into infinite loop. User can modify the output error message
   * @retval None
   */
	 
	 //AT TAs & STUDENTS
	 //Feel free to use or discard this function, you can add more Error definitions and assign 
	 //them codes in the supporting_functions.h file
	 
	 //Many functions in the drivers return a status code, usually HAL_OK or HAL_ERROR. You can check
	 //the output of your driver function call and either proceed or call this function to print
	 //whatever error message you want. Helps you in debugging your code at many points during developing
	 //your codes
	 
LIS3DSH_InitTypeDef LIS3DSH_InitStruct;
LIS3DSH_DRYInterruptConfigTypeDef LIS3DSH_IntConfigStruct;

int16_t PITCH = 0, ROLL = 0;

/* Fucntion : Error_Handler;
* Input : uint16_t error_code
* Decription: Displays an error message when an error is detected,
*/
void Error_Handler			(uint16_t error_code){
	//User error handling code, could use printf to relay information to user
	switch (error_code){	
		case RCC_CONFIG_FAIL: 
			printf("ERROR: Error in initialising System Clocks \n");
			break;
		case TIM_INIT_FAIL: 
			printf("ERROR: Error in initialising Timer base \n");
			break;
		case ADC_INIT_FAIL: 
			printf("ERROR: Error in initialising ADC \n");
			break;
		case ADC_CH_CONFIG_FAIL: 
			printf("ERROR: Error in initialising ADC Channel Configuration \n");
			break;
		case ADC_MULTIMODE_FAIL: 
			printf("ERROR: Error in initialising ADC Multimode\\DMA \n");
			break;
		case EXTI_SPI1_FAIL: 
			printf("ERROR: Error in initialising SPI1 \n");
			break;
		default: printf ("ERROR: AN ERROR OCCURED \n");
		break;
	}
}

/* Function : LIS3DSH_InterruptConfigStruct
* Input : LIS3DSH_DRYInterruptConfigTypeDef *LIS3DSH_IntConfigStruct
* Description : 
*/
void LIS3DSH_InterruptConfigStruct(LIS3DSH_DRYInterruptConfigTypeDef *LIS3DSH_IntConfigStruct){
	LIS3DSH_IntConfigStruct->Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	LIS3DSH_IntConfigStruct->Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
	LIS3DSH_IntConfigStruct->Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
}

/* Function: Init_NVIC_Interrupt()
* Input : IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority
* Description: General function that initialises the interrupt for a specific function
*/
void Init_NVIC_Interrupt(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority){
	HAL_NVIC_SetPriority(IRQn, PreemptPriority, SubPriority);
	HAL_NVIC_EnableIRQ(IRQn);
}

/* Function: KeyBouncingDelay()
* Input: GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState pin_state, uint8_t rise_edge
* Description: Function in charge of elliminating the key debouncing when the keypad buttons are pressed.
*/
void KeyBouncingDelay(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState pin_state, uint8_t rise_edge){
	while(rise_edge) {
		rise_edge = (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == pin_state);
	}
}

/* Function : Init_ACC
* Description: Initialises the accelerometer peripheral such that it will acquire data at 25 HZ and
* initialises the interrupt to be used by the routine.
*/
void Init_ACC(void){
	__HAL_RCC_GPIOE_CLK_ENABLE();
	LIS3DSH_InitStruct.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;
	LIS3DSH_InitStruct.Axes_Enable = LIS3DSH_XYZ_ENABLE;
	LIS3DSH_InitStruct.Continous_Update = LIS3DSH_ContinousUpdate_Disabled;
	LIS3DSH_InitStruct.Full_Scale = LIS3DSH_FULLSCALE_2;
	LIS3DSH_InitStruct.AA_Filter_BW = LIS3DSH_AA_BW_800;
	LIS3DSH_InitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;
	
	LIS3DSH_Init(&LIS3DSH_InitStruct);

	LIS3DSH_IntConfigStruct.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	LIS3DSH_IntConfigStruct.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
	LIS3DSH_IntConfigStruct.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
	
	LIS3DSH_DataReadyInterruptConfig(&LIS3DSH_IntConfigStruct);
	
	Init_NVIC_Interrupt(EXTI0_IRQn, 0, 3);
}

/* Function : Init_Read_Keypad()
* Description : Initialises the GPIOs for the keypad as well as the interrupt to be used for this routine
*/
void Init_Read_Keypad(void){
		InitReadButton();
		Init_NVIC_Interrupt(EXTI9_5_IRQn, 0, 2);
}

/* Function : setPitch()
* Input : int16_t pitch
* Description : Sets the pitch value to be acheived that was inputted by the user.
*/
void setPitch(int16_t pitch){
	PITCH = pitch;
}

/* Function : getPitch
* Returns : int16_t PITCH
* Description: Returns the value of the PITCH calculated from the accelerometer data
*/
int16_t getPitch(void){
	return PITCH;
}

/* Function : setRoll()
* Input : int16_t roll
* Description : Sets the roll value to be acheived that was inputted by the user.
*/
void setRoll(int16_t roll){
	ROLL = roll;
}

/* Function : getRoll
* Returns : int16_t ROLL
* Description: Returns the value of the ROLL calculated from the accelerometer data
*/
int16_t getRoll(void){
	return ROLL;
}
