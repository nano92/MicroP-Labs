#include <stdio.h>
#include <time.h>
#include "main.h"

// Calls the assembly code for the FIR filter
extern void FIR_asm();

/* Function   : FIR_C
 * Input      : float32_t* InputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order
 * Output     : float32_t* OutputArray
 * Returns    : ??
 * Description: Runs a C based FIR filter over the input array values of InputArray by only consifering the vlaues after the 
 * Order-th element of said array. Then, stores the FIR filter results in the output array OutputArray
 */
int32_t FIR_C(float32_t* InputArray, float32_t* FIR_coeff, int32_t Length, int32_t Order, float32_t* OutputArray){
	
	for(int32_t i = 0; i < Length - Order; i++){ 
		float32_t sum = 0;
		for(int32_t j = 0; j <= Order; j++){
			sum = sum  + ((InputArray[i + j]) * (FIR_coeff[j]));
		}
		OutputArray[i] = sum;
	}
	return 0;
}

/* Function   : FIR_CMSIS
 * Input      : float32_t* InputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order
 * Output     : float32_t* OutputArray
 * Returns    : ??
 * Description: Runs a FIR filter using the CMSIS DSP library commands over the input array values of InputArray. 
 * Then, stores the FIR filter results in the output array OutputArray
 */
int32_t FIR_CMSIS(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order){
	
	arm_fir_instance_f32 S;
	
	float32_t FIR_state[Length + Order - 1];
	
	arm_fir_init_f32(&S, Order, &FIR_coeff[0], &FIR_state[0], Length);
	
	arm_fir_f32(&S, InputArray, OutputArray, Length);
	
	return 0;
}

/* Function   : Data_Sub
 * Input      : float32_t* InputArray, float32_t* FilterArray, uint32_t Length, uint32_t Order, uint16_t CMSIS
 * Output     : float32_t* ResultArray
 * Description: Calculates the difference between each element of the InputArray and FilterArray, and stores each result into the 
 * output array ResultArray
 */
int32_t Data_Sub(float32_t* InputArray, float32_t* FilterArray, uint32_t Length, uint32_t Order, uint16_t CMSIS, float32_t* ResultArray){
	uint32_t j = (CMSIS == 1) ? Order : 0;
	for(uint32_t i = 0; i < Length - Order; i++){
		ResultArray[i] = InputArray[i] - FilterArray[j];
		j++;
	}
	return 0;
}
/* Function   : Sum_Array
 * Input      : float32_t* A, uint32_t Length, uint16_t CMSIS, uint32_t Order
 * Output     : float32_t* sum
 * Returns    : -1 if size of the array is not equal to length
 * Description: Adds all the element of the input array (A) and stores the value in the variable sum
 */
int32_t Sum_Array(float32_t* A, uint32_t Length, float32_t* sum, uint16_t CMSIS, uint32_t Order){
	uint32_t j = (CMSIS == 1) ? Order : 0;
	
	uint32_t A_length = sizeof(A)/sizeof(A[0]);
	if(Length < A_length){
		return -1;
	}
	
	for(uint32_t i = j; i < Length + j; i++){
		*sum = A[i] + *sum;
	}
	return 0;
}


/* Function   : stdev
 * Input      : float32_t* Input, int32_t LmO, uint16_t CMSIS, uint32_t Order
 * Output     : float32_t* std, float32_t* mavg
 * Returns    : -1 when an error is encounter, 0 otherwise
 * Description: Calculates the standard deviation and mean average for an array of input values. 
 *	The underlying algorithm is used for standard deviation:
 *
 * std = sqrt((sumOfSquares - sum^2 / LmO) / (LmO - 1))
 *   where, sumOfSquares = Input[0] * Input[0] + Input[1] * Input[1] + ... + Input[blockSize-1] * Input[blockSize-1]
 *                   sum = Input[0] + Input[1] + Input[2] + ... + Input[blockSize-1]
 */
int32_t stdev(float32_t* Input, int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* std,  float32_t* mavg) {
	// Identifies if the Input array comes from CMSIS filter, since its actual filter values start at the Order-th place
	uint32_t j = (CMSIS == 1) ? Order : 0;
	
	// Local values of the function
	float32_t sum = 0;      // Retains the sum of all the input array elements
	float32_t sumOfmul = 0; // Retains the value of all the variance array element
	float32_t covar[LmO];   // Retains the value of all the variance of for each input array element in an array format. LmO = Length minus Order
	
	// Generates the mean average and detects error in the sum
	int32_t error = Sum_Array(Input, LmO, &sum, CMSIS, Order); 
	if (error < 0) {return -1;}
	*mavg = sum/(float32_t)LmO;
	
	// Computes the standard deviation
	for (uint32_t i = 0; i < LmO; i++) {
		covar[i] = Input[j] * Input[j];
		j++;
	}
	error = Sum_Array(covar, LmO, &sumOfmul, 0, 0);
	if (error < 0) {return -1;} 
	float32_t meanOfsquare = sumOfmul/(float32_t)(LmO - 1);
	float32_t squareOfmavg = ((sum * sum)/(float32_t)LmO)/(float32_t)(LmO - 1);	
	*std = sqrt(meanOfsquare - squareOfmavg);
	
	return 0;
}
/* Function   : corr_coeff
 * Input      : float32_t* Input, float32_t* FIR_result, int32_t LmO, uint16_t CMSIS, uint32_t Order
 * Output     : float32_t* correlation
 * Description: Calculates the correlation between the input arrays Input and FIR_result, and later pass the result to correlation.
 */

int32_t corr_coeff(float32_t* Input, float32_t* FIR_result, int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* correlation) {
	uint32_t j = (CMSIS == 1) ? Order : 0;
	
	float32_t covar_Sum, std_FIR, std_IN, input_mavg, result_mavg = 0;
	float32_t covar_FIR[LmO], covar_IN[LmO], covar_R[LmO];
		
	int32_t error = stdev(Input, LmO, 0, 0, &std_IN, &input_mavg);
	if (error < 0) {return -1;}
	
	error = stdev(FIR_result, LmO, CMSIS, Order, &std_FIR, &result_mavg);
	if (error < 0) {return -1;}
	
	for(int32_t i = 0; i < LmO; i++){
		covar_R[i] = ((Input[i] - input_mavg)/std_IN) * ((FIR_result[i + j] - result_mavg)/std_FIR);
	}
	
	covar_Sum = 0;
	error = Sum_Array(covar_R, LmO, &covar_Sum, 0, 0);
	if (error < 0) {return -1;}
	
	*correlation = covar_Sum/(float32_t)LmO; 
	
	return 0;
}

/* Function   : CMSIS_DSP_lib
 * Input      : float32_t* InputArray, float32_t* FIR_result, uint32_t Length
 * Output     : float32_t* sub, float32_t* std, float32_t* mean, float32_t* Corr_Array
 * Description: Calculates the difference, standard deviation, average and correlation from PART B by using the arm libraries 
 */
int32_t CMSIS_DSP_lib(float32_t* InputArray, float32_t* FIR_result, uint32_t Length, float32_t* sub, float32_t* std, float32_t* mean, float32_t* Corr_Array){
	arm_sub_f32(InputArray, FIR_result, sub, Length);
	
	arm_std_f32(sub, Length, std);
	
	arm_mean_f32(sub, Length, mean);
	
	arm_correlate_f32(InputArray, Length, FIR_result, Length , Corr_Array);
	
	return 0;
	
}

/* Function   : testbench
 * Description: Runs all the required tasks for the purpose of lab1
 */
int32_t testbench()
{	
	// PART I
	puts("PART I\n");
	srand(time(NULL));
	
	// Stablishes the local variables for execution
	const uint32_t length = 7;
	const uint32_t order = 5;
	float32_t FIR_coeff[order] = {0.1, 0.15, 0.5, 0.15, 0.1};	
	float32_t InputArray[length] = {0.5, 0.9, 0.34, 0.69, 0.34, 0.12, 0.89};
	float32_t OutputArray_ASM[length];
	float32_t OutputArray_C[length];
	float32_t OutputArray_CMSIS[length];

	// Runs the input array into each type of FIR filter
	FIR_asm(InputArray, OutputArray_ASM, length, FIR_coeff);	
	FIR_C(InputArray, FIR_coeff, length, 4, OutputArray_C);	
	FIR_CMSIS(InputArray, OutputArray_CMSIS, FIR_coeff, length, order);
	
	// Prints out the values obtained from each filter implementation
	for(int i = 0; i < length; i++){
		printf("OA_C[%d] = %.4f\tOA_ASM[%d] = %.4f\tOA_CMSIS[%d] = %.4f\n", 
			i, OutputArray_C[i], i, OutputArray_ASM[i], i, OutputArray_CMSIS[i]);
	}
	
	//PART II
	puts("\nPART II - A\n");
	// a) Get the difference between the input and the filtered value
	int32_t size = length-order+1;
	float32_t sub_ASM[size], sub_C[size], sub_CMSIS[size]; 
	
	// Difference with respect to the assembly based FIR filter and the input
	int32_t error = Data_Sub(InputArray, sub_ASM, length, order-1, 0, OutputArray_ASM);
	if (error < 0) { printf("Error in data sub assembly"); return -1;}
	
	// Difference with respect to the C based FIR filter and the input
	error = Data_Sub(InputArray, sub_C, length, order-1, 0, OutputArray_C);
	if (error < 0) { printf("Error in data sub C"); return -1;}
	
	// Difference with respect to the CMSIS FIR filter and the input
	error = Data_Sub(InputArray, sub_CMSIS, length, order-1, 1, OutputArray_CMSIS);
	if (error < 0) { printf("Error in data sub CMSIS"); return -1;}
	
	// Prints out the results obtained from the 3 substraction processes
	for(int32_t i = 0; i < size; i++){
		printf("Substraction: sub_ASM[%d] = %.4f\tsub_C[%d] = %.4f\tsub_CMSIS[%d] = %.4f\n", 
		i, sub_ASM[i], i, sub_C[i], i, sub_CMSIS[i]);
	}	
	
	// b) Get the standard deviation and average for each difference
	float32_t std_ASM, std_C, std_CMSIS;
	float32_t mavg_ASM, mavg_C, mavg_CMSIS;
	
	// Calculating the standard deviation and average for the assembly based difference
	error = stdev(sub_ASM, size, 0, 0, &std_ASM, &mavg_ASM);
	if (error < 0) { printf("Error in stdev assembly"); return -1;}
	
	// Calculating the standard deviation and average for the C based difference
	error = stdev(sub_C, size, 0, 0, &std_C, &mavg_C);
	if (error < 0) { printf("Error in stdev C"); return -1;}
	
	// Calculating the standard deviation and average for the CMSIS FIR difference
	error = stdev(sub_CMSIS, size, 0, 0, &std_CMSIS, &mavg_CMSIS);
	if (error < 0) { printf("Error in stdev CMSIS"); return -1;}
	
	// Prints the results obtained for each standard deviation and average
	printf("Standard deviation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", std_ASM, std_C, std_CMSIS);
	printf("Mean average: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n",mavg_ASM, mavg_C, mavg_CMSIS);
	
	// c) Calculate the correlation between the input and the different FIR filters type
	float32_t corr_ASM, corr_C, corr_CMSIS;
	float32_t correlation[size * 2];
	
	// Calculate the correlation between the input and assembly based FIR filter's output values
	error = corr_coeff(InputArray, OutputArray_ASM, size, 0, 0, &corr_ASM);
	if (error < 0) { printf("Error in correlation assembly"); return -1;}
	
	// Calculate the correlation between the input and C based FIR filter's output values
	error = corr_coeff(InputArray, OutputArray_C, size, 0, 0, &corr_C);
	if (error < 0) { printf("Error in correlation C"); return -1;}
	
	// Calculate the correlation between the input and CMSIS FIR filter's output values
	error = corr_coeff(InputArray, OutputArray_CMSIS, size, 1, order - 1, &corr_CMSIS);
	if (error < 0) { printf("Error in correlation CMSIS"); return -1;}
	
	// Prints the results obtained for each correlation 
	printf("Correlation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", corr_ASM, corr_C, corr_CMSIS);
	
	// Repeat of PART II by using the CMSIS-DSP Library 
	puts("\nPART II - B\n");
	float32_t OutputArray_CMSIS_lib[size], sub_ASM_lib[size], sub_C_lib[size], sub_CMSIS_lib[size], std_ASM_lib, std_C_lib, std_CMSIS_lib; 
	float32_t mavg_ASM_lib, mavg_C_lib, mavg_CMSIS_lib, corr_ASM_lib[size*2], corr_C_lib[size*2], corr_CMSIS_lib[size*2];
	
	// Calculating the difference, standard deviation, average and correlation for each filter
	CMSIS_DSP_lib(InputArray, OutputArray_ASM, size, sub_ASM_lib, &std_ASM_lib, &mavg_ASM_lib, corr_ASM_lib);
	CMSIS_DSP_lib(InputArray, OutputArray_C, size, sub_C_lib, &std_C_lib, &mavg_C_lib, corr_C_lib);
	for(int32_t i = 0; i < size; i++){
		OutputArray_CMSIS_lib[i] = OutputArray_CMSIS[i + order - 1];
	}
	CMSIS_DSP_lib(InputArray, OutputArray_CMSIS_lib, size, sub_CMSIS_lib, &std_CMSIS_lib, &mavg_CMSIS_lib, corr_CMSIS_lib);
	
	for(int32_t i = 0; i < size; i++){
		printf("Substraction: sub_ASM_lib[%d] = %.4f\tsub_C_lib[%d] = %.4f\tsub_CMSIS_lib[%d] = %.4f\n", 
		i, sub_ASM_lib[i], i, sub_C_lib[i], i, sub_CMSIS_lib[i]);
	}
	
	// Prints the results obtained for the standard deviation and average
	printf("Standard deviation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", std_ASM_lib, std_C_lib, std_CMSIS_lib);
	printf("Mean average: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n",mavg_ASM_lib, mavg_C_lib, mavg_CMSIS_lib);
	
	return 0;
}

int main(){
	testbench();
	
	return 0;
}