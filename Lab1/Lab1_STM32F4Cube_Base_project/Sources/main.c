#include <stdio.h>
#include "arm_math.h"

float Sum_Array(float* InputArray, float* FIR_coeff, int Order, int Location) {
	float sum = 0;
	for(int j = Order; j >= 0; j--) {
		sum = sum + InputArray[Location - j] * (FIR_coeff[j]);
	}		
	return sum;
}

int FIR_C(float* InputArray, float* OutputArray, float* FIR_coeff, int Length, int Order){
		
	for(int i = Length - 1; i >= Order; i--){
		OutputArray[i] = Sum_Array(InputArray, FIR_coeff, Order, i);
	}
		
	return 0;
}

int FIR_CMSIS(float* InputArray, float* OutputArray, float* FIR_coeff, int Length, int Order){
	arm_fir_instance_f32 S;
	
	return 0;
}

int main()
{
	float FIR_coeff[5] = {0.1, 0.15, 0.5, 0.15, 0.1};	
	

	return 0;
}
