/**
  ******************************************************************************
  * File Name          : timer.c
  * Description        : Init Timer in PWM mode and handle its pulse
	* Authors						 : Luis Gallet
  * Group              : 10	
	* Version            : 1.0.0
	* Date							 : March 10th, 2017
  ******************************************************************************
  */
	
#include "timer.h"

static HAL_StatusTypeDef TIM_PWM_ConfigStart(uint32_t channel);
static HAL_StatusTypeDef Set_LED_Pulse(int16_t angle, int16_t angle_difference, 
															uint32_t first_channel, uint32_t second_channel);

TIM_HandleTypeDef handle_tim4, handle_tim3;
TIM_OC_InitTypeDef init_OC_tim;

/* Function : Init_TIM1_Config
 * Input    : None
 * Returns	: HAL status 
 * Description: Initialize TIM1 timer in Interrupt mode
*/
HAL_StatusTypeDef Init_TIM3_Config(void){
	TIM_Base_InitTypeDef init_tim;
	HAL_StatusTypeDef status;
	
	__TIM3_CLK_ENABLE();
	
	init_tim.Prescaler = 420; 
	init_tim.CounterMode = TIM_COUNTERMODE_UP;
	/*       
    TIM_Period = timer_tick_frequency / PWM_frequency - 1
    
    In our case, for 100 Hz PWM_frequency, set Period to
    
    TIM_Period = 42000000 / 100 - 1 = 419999 = (prescalar + 1)*period
*/
	init_tim.Period = 1000;
	init_tim.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	init_tim.RepetitionCounter = 0;
	
	handle_tim3.Init = init_tim;
	handle_tim3.Instance = TIM3;
	
	status = HAL_TIM_Base_Init(&handle_tim3);
	if(status != HAL_OK){
		printf("HAL_TIM_Base_Init status: %d\n", status);
		return status;
	}
	
	status = HAL_TIM_Base_Start_IT(&handle_tim3);
	if(status != HAL_OK){
		printf("HAL_TIM_Base_Init status: %d\n", status);
		return status;
	}
	//Init_NVIC_Interrupt(TIM3_IRQn, 0, 2);
}

/* Function : Init_TIM4_Config
 * Input    : None
 * Returns	: HAL status 
 * Description: Initialize TIM4 timer in PWM mode and enbale it to control LEDs
*/
HAL_StatusTypeDef Init_TIM4_Config(void){
	TIM_Base_InitTypeDef init_tim;
	
	HAL_StatusTypeDef status;
	
	__TIM4_CLK_ENABLE();
	
	init_tim.Prescaler = 117; //0
	init_tim.CounterMode = TIM_COUNTERMODE_UP;
	/*       
    TIM_Period = timer_tick_frequency / PWM_frequency - 1
    
    In our case, for 2K Hz PWM_frequency, set Period to
    
    TIM_Period = 84000000 / 2000 - 1 = 41999 = (prescalar + 1)*period
*/
	init_tim.Period = 360; //41999
	init_tim.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	init_tim.RepetitionCounter = 0;
	
	handle_tim4.Init = init_tim;
	handle_tim4.Instance = TIM4;
	
	status = HAL_TIM_PWM_Init(&handle_tim4);
	if(status != HAL_OK){
		printf("HAL_TIM_Base_Init status: %d\n", status);
		return status;
	}
		
	init_OC_tim.OCMode = TIM_OCMODE_PWM1;
	init_OC_tim.OCNPolarity = TIM_OCPOLARITY_HIGH;
	init_OC_tim.OCIdleState = TIM_OCIDLESTATE_RESET;
	init_OC_tim.Pulse = 0; 
	
	return status;
}

/* Function : Set_LEDBrightness
 * Input    : acc_roll_angle, acc_pitch_angle, roll_angle, pitch_angle
 * Returns	: HAL status 
 * Description: Set LED britghtness depending on the angle mesured by the acc 
 * and the one entered by the user
*/
HAL_StatusTypeDef Set_LEDBrightness(int16_t acc_roll_angle, 
							int16_t acc_pitch_angle, int16_t roll_angle, int16_t pitch_angle){
	
	HAL_StatusTypeDef status;
	
	int16_t roll_difference = acc_roll_angle - roll_angle;
	int16_t pitch_difference = acc_pitch_angle - pitch_angle;
	
	if(roll_difference < 0){
		roll_difference = roll_difference * -1;
	}
	if(pitch_difference < 0){
		pitch_difference = pitch_difference * -1;
	}
	
	if(pitch_difference < 5){
		pitch_difference = 0;
	}
	
	if(roll_difference < 5){
		roll_difference = 0;
	}
	
	status = Set_LED_Pulse(acc_roll_angle, roll_difference, TIM_CHANNEL_2, TIM_CHANNEL_4);
	if(status != HAL_OK){
		printf("Set_LED_Pulse (roll) status: %d\n", status);
		return status;
	}
	
	status = Set_LED_Pulse(acc_pitch_angle, pitch_difference, TIM_CHANNEL_1, TIM_CHANNEL_3);
	if(status != HAL_OK){
		printf("Set_LED_Pulse (pitch) status: %d\n", status);
		return status;
	}
	
	return status;
}

/* Function : Set_LED_Pulse
 * Input    : angle, angle_difference, first_channel, second_channel
 * Returns	: HAL status 
 * Description: Calculates the LED brightness depending on the angle and the
 * angle difference
*/
HAL_StatusTypeDef Set_LED_Pulse(int16_t angle, int16_t angle_difference, uint32_t first_channel, uint32_t second_channel){
	HAL_StatusTypeDef status;
	if(angle >= 90 && angle <= 270){
		init_OC_tim.Pulse = angle_difference;
		
		status = TIM_PWM_ConfigStart(first_channel);
		if(status != HAL_OK){
			printf("TIM_PWM_ConfigStart status: %d\n", status);
			return status;
		}
				
		init_OC_tim.Pulse = 360 - angle_difference;

		status = TIM_PWM_ConfigStart(second_channel);
		if(status != HAL_OK){
			printf("TIM_PWM_ConfigStart status: %d\n", status);
			return status;
		}
		
	}else{
		init_OC_tim.Pulse = 360 - angle_difference;
		
		status = TIM_PWM_ConfigStart(first_channel);
		if(status != HAL_OK){
			printf("TIM_PWM_ConfigStart status: %d\n", status);
			return status;
		}
				
		init_OC_tim.Pulse = angle_difference;
		
		status = TIM_PWM_ConfigStart(second_channel);
		if(status != HAL_OK){
			printf("HAL_TIM_PWM_ConfigChannel status: %d\n", status);
			return status;
		}
	}
	return status;
}

/* Function : TIM_PWM_ConfigStart
 * Input    : channel
 * Returns	: HAL status 
 * Description: Sets the brightness to the chosen channel(LED)
*/
HAL_StatusTypeDef TIM_PWM_ConfigStart(uint32_t channel){
	HAL_StatusTypeDef status;
	status = HAL_TIM_PWM_ConfigChannel(&handle_tim4, &init_OC_tim, channel);
	if(status != HAL_OK){
		printf("HAL_TIM_PWM_ConfigChannel status: %d\n", status);
		return status;
	}
	
	status = HAL_TIM_PWM_Start(&handle_tim4, channel);
	if(status != HAL_OK){
		printf("HAL_TIM_PWM_ConfigChannel status: %d\n", status);
		return status;
	}
	
	return status;
}