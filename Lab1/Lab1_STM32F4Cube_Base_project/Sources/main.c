#include <math.h>
#include "main.h"
#include "testbench.h"
 

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
int32_t stdev(float32_t* Input, const int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* covar, float32_t* std,  float32_t* mavg) {
	// Identifies if the Input array comes from CMSIS filter, since its actual filter values start at the Order-th place
	uint32_t j = (CMSIS == 1) ? Order : 0;
	//Reset covar[] temporary variable
	//Retains the value of all the variance of for each input array element in an array format. LmO = Length minus Order
	memset(covar, 0, LmO);
	// Local values of the function
	float32_t sum = 0;      // Retains the sum of all the input array elements
	float32_t sumOfmul = 0; // Retains the value of all the variance array element
	float32_t mul = 0;
	// Generates the mean average and detects error in the sum
	int32_t error = Sum_Array(Input, LmO, &sum, CMSIS, Order); 
	if (error < 0) {return -1;}
	*mavg = sum/(float32_t)LmO;
	
	// Computes the standard deviation
	for (uint32_t i = 0; i < LmO; i++) {
		mul = Input[j];
		covar[i] = pow(mul, 2);
		j++;
	}
	error = Sum_Array(covar, LmO, &sumOfmul, 0, 0);
	if (error < 0) {return -1;} 
	
	float32_t temp = fabsf((sumOfmul - ((sum * sum)/LmO)));
	*std = sqrt(temp / (LmO - 1) );

	return 0;
}
/* Function   : corr_coeff
 * Input      : float32_t* Input, float32_t* FIR_result, int32_t LmO, uint16_t CMSIS, uint32_t Order
 * Output     : float32_t* correlation
 * Description: Calculates the correlation coeffcient between the input arrays Input and FIR_result.
 */

int32_t corr_coeff(float32_t* Input, float32_t* FIR_result, const int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* covar, float32_t* correlation) {
	uint32_t j = (CMSIS == 1) ? Order : 0;
	float32_t covar_Sum, std_FIR, std_IN, input_mavg, result_mavg;
		
	int32_t error = stdev(Input, LmO, 0, 0, covar, &std_IN, &input_mavg);
	if (error < 0) {return -1;}
	
	error = stdev(FIR_result, LmO, CMSIS, Order, covar, &std_FIR, &result_mavg);
	if (error < 0) {return -1;}
	
	//Reset covar[] temporary variable
	memset(covar, 0, LmO);
	
	for(int32_t i = 0; i < LmO; i++){
		covar[i] = ((Input[i] - input_mavg)/std_IN) * ((FIR_result[i + j] - result_mavg)/std_FIR);
	}
	
	covar_Sum = 0;
	error = Sum_Array(covar, LmO, &covar_Sum, 0, 0);
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

int main(){
	if(testbench() < 0){ 
		puts("Testbench failed"); 
	}else{
		puts("Testbench succeeded");
	}
	
	return 0;
}