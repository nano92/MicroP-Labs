#include <stdio.h>
#include <time.h>
#include "main.h"

extern int FIR_asm();

int32_t FIR_C(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, int32_t Length, int32_t Order){
	
	for(int32_t i = 0; i < Length - Order; i++){ 
		float32_t sum = 0;
		for(int32_t j = 0; j <= Order; j++){
			//printf("Input[%d] = %.4f\n",i - j,InputArray[i-j]);
			//printf("j = %d\n", j);
			//printf("Coeff[%d] = %.4f\n",j,FIR_coeff[j]);
			sum = sum  + ((InputArray[i + j]) * (FIR_coeff[j]));
		}
		//printf("Loop finished");
		OutputArray[i] = sum;
	}
	return 0;
}

int FIR_ASM(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order){
	return FIR_asm(InputArray, OutputArray, Length, FIR_coeff);
}

int32_t FIR_CMSIS(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order){
	
	arm_fir_instance_f32 S;
	
	float32_t FIR_state[Length + Order - 1];
	
	arm_fir_init_f32(&S, Order, &FIR_coeff[0], &FIR_state[0], Length);
	
	arm_fir_f32(&S, InputArray, OutputArray, Length);
	
	return 0;
}

int32_t testbench()
{	
	srand(time(NULL));
	
	float32_t FIR_coeff[5] = {0.1, 0.15, 0.5, 0.15, 0.1};	
	float32_t InputArray[10]; //= {0.5, 0.9, 0.34, 0.69, 0.34, 0.12, 0.89};
	float32_t OutputArray_ASM[10];
	float32_t OutputArray_C[10];
	float32_t OutputArray_CMSIS[10];
	for(int32_t i = 0; i < 10; i++){
		InputArray[i] = (float)sin((double)i);
		//printf("InputArray[%d] = %.4f\n", i, InputArray[i]);
	}
	
	//puts("Modified");
	for(int32_t j = 0; j < 1; j++){
		int r = rand() % 10;
		InputArray[r] = InputArray[r] + (float32_t)(rand() % 4);
		//printf("InputArray[%d] = %.4f\n", r, InputArray[r]);
	}
	
	FIR_ASM(InputArray, OutputArray_ASM, FIR_coeff, 10, 4);
	
	FIR_C(InputArray, OutputArray_C, FIR_coeff, 10, 4);
	FIR_CMSIS(InputArray, OutputArray_CMSIS, FIR_coeff, 10, 5);
	
	for(int i = 0; i < 10; i++){
		printf("OA_C[%d] = %.4f\tOA_ASM[%d] = %.4f\tOA_CMSIS[%d] = %.4f\n", 
			i, OutputArray_C[i], i, OutputArray_ASM[i], i, OutputArray_CMSIS[i]);
	}

	return 0;
}

int32_t main(){
	testbench();
	
	return 0;
}