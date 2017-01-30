#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "main.h"


int32_t main()
{	
	float32_t FIR_coeff[5] = {0.1, 0.15, 0.5, 0.15, 0.1};	
	float32_t InputArray[100];
	for(uint32_t i = 0; i < 100; i++){
		InputArray[i] = (float)sin((double)i);
		printf("InputArray[%d] = %f\n", i, InputArray[i]);
	}
	
	puts("Modified");
	for(uint32_t j = 0; j < 10; j++){
		int r = rand() % 100;
		InputArray[r] = InputArray[r] + (float32_t)j * 0.5;
		printf("InputArray[%d] = %f\n", r, InputArray[r]);
	}
	
	return 0;
}