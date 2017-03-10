/**
  ******************************************************************************
  * File Name          : supporting_functions.c
  * Description        : User defined common functions that can be used across projects
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
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

void LIS3DSH_InterruptConfigStruct(LIS3DSH_DRYInterruptConfigTypeDef *LIS3DSH_IntConfigStruct){
	LIS3DSH_IntConfigStruct->Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
	LIS3DSH_IntConfigStruct->Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
	LIS3DSH_IntConfigStruct->Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
}

void Init_NVIC_Interrupt(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority){
	HAL_NVIC_SetPriority(IRQn, PreemptPriority, SubPriority);
	HAL_NVIC_EnableIRQ(IRQn);
}

void KeyBouncingDelay(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState pin_state, uint8_t rise_edge){
	while(rise_edge) {
		rise_edge = (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == pin_state);
	}
}

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

void Init_Read_Keypad(void){
		InitReadButton();
		Init_NVIC_Interrupt(EXTI9_5_IRQn, 0, 2);
}

void setPitch(int16_t pitch){
	PITCH = pitch;
}
int16_t getPitch(void){
	return PITCH;
}

void setRoll(int16_t roll){
	ROLL = roll;
}
int16_t getRoll(void){
	return ROLL;
}
