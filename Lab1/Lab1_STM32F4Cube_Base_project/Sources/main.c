#include <stdio.h>
#include "arm_math.h"

typedef struct   
{
	int coeff_lenght;
	float b[];
}FIR_coeff;

int FIR_C(float* InputArray, float* OutputArray, FIR_coeff* coeff, int Length, int Order){
		
	for(int i = Length - 1; i >= Order; i--){
		OutputArray[i] = (coeff->b0 * InputArray[i])+ 
											(coeff->b1 * InputArray[i - (Order - 3)])+ 
												(coeff->b2 * InputArray[i - (Order - 2)])+ 
													(coeff->b3 * InputArray[i - (Order - 1)])+ 
														(coeff->b4 * InputArray[i - (Order)]);
		
	}
	
	return 0;
}



int main()
{
	FIR_coeff coeff;
	
	coeff.b[] = {0.1, 0.15, 0.5, 0.15, 0.1};
	
	return 0;
}
