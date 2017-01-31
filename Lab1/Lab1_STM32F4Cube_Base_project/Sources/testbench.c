#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "testbench.h"


int32_t testbench()
{	
	srand(time(NULL));
	
	float32_t FIR_coeff[5] = {0.1, 0.15, 0.5, 0.15, 0.1};	
	float32_t InputArray[100];
	float32_t OutputArray_C[100];
	float32_t OutputArray_CMSIS[100];
	for(uint32_t i = 0; i < 100; i++){
		InputArray[i] = (float)sin((double)i);
		//printf("InputArray[%d] = %.4f\n", i, InputArray[i]);
	}
	
	//puts("Modified");
	for(uint32_t j = 0; j < 10; j++){
		int r = rand() % 100;
		InputArray[r] = InputArray[r] + (float32_t)(rand() % 4);
		//printf("InputArray[%d] = %.4f\n", r, InputArray[r]);
	}
	
	FIR_C(&InputArray, &OutputArray_C, &FIR_coeff, 100, 4);
	FIR_CMSIS(&InputArray, &OutputArray_CMSIS, &FIR_coeff, 100, 4);
	
	for(int i = 0; i < 100; i++){
		printf("OA_C[%d] = %.4f\tOA_CMSIS[%d] = %.4f\n", i, OutputArray_C[i], i, OutputArray_CMSIS[i]);
	}
	
	
	return 0;
}