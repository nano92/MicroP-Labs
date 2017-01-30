#include <stdio.h>
#include "main.h"


float32_t Window_Sum(float32_t* InputArray, float32_t* FIR_coeff, uint32_t Order, uint32_t Location){
	float32_t sum = 0;
	for(uint32_t j = Order; j >= 0; j--) {
		sum = sum + InputArray[Location - j] * (FIR_coeff[j]);
	}		
	return sum;
}

int32_t FIR_C(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order){
		
	for(uint32_t i = Length - 1; i >= Order; i--){
		OutputArray[i] = Window_Sum(InputArray, FIR_coeff, Order, i);
	}
		
	return 0;
}

int32_t FIR_CMSIS(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order){
	
	arm_fir_instance_f32 S;
	
	float32_t FIR_state[Length + Order];
	
	arm_fir_init_f32(&S, Order, &FIR_coeff[0], &FIR_state[0], Length);
	
	arm_fir_f32(&S, InputArray, OutputArray, Length);
	
	return 0;
}