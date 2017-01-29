#include <stdio.h>
#include "arm_math.h"

typedef struct   
{
	float b0;
	float b1;
	float b2;
	float b3;
	float b4;
}FIR_coeff;

int FIR_C(float* InputArray, float* OutputArray, FIR_coeff* coeff, int Length, int Order){
	
	return 0;
}

float sumArray(float* InputArray, FIR_coeff* coeff, int Order) {
	float sum = 0;
	for(int j = Order; j>=0; j--) {
		sum = sum + InputArray[j]*coeff->b[j];
	}		
	return sum;
}

int main()
{
	return 0;
}
