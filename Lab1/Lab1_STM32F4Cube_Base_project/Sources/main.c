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

float sumArray(float* InputArray, FIR_coeff* coeff, int Order, int Location) {
	float sum = 0;
	for(int j = Order; j>=0; j--) {
		sum = sum + InputArray[Location-j]*coeff->b[j];
	}		
	return sum;
}

int main()
{
<<<<<<< HEAD
	FIR_coeff coeff;
	
	coeff.b[] = {0.1, 0.15, 0.5, 0.15, 0.1};
	
=======
>>>>>>> d0bc1661cb9bf0305ed8391c329934b1736b707d
	return 0;
}
