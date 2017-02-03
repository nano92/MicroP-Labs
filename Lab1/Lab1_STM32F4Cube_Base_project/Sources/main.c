#include <stdio.h>
#include <time.h>
#include "main.h"

extern int FIR_asm();

int32_t FIR_C(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, int32_t Length, int32_t Order){
	
	for(int32_t i = 0; i < Length - Order; i++){ 
		float32_t sum = 0;
		for(int32_t j = 0; j <= Order; j++){
			sum = sum  + ((InputArray[i + j]) * (FIR_coeff[j]));
		}
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

int32_t Data_Sub(float32_t* InputArray, float32_t* OutputArray, float32_t* ResultArray, uint32_t Length, uint32_t Order, uint16_t CMSIS){
	uint32_t j = (CMSIS == 1) ? Order : 0;
	for(uint32_t i = 0; i < Length - Order; i++){
		ResultArray[i] = InputArray[i] - OutputArray[j];
		j++;
	}
	return 0;
}

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

int32_t stdev(float32_t* Input, int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* std,  float32_t* mavg) {
	uint32_t j = (CMSIS == 1) ? Order : 0;
	float32_t sum = 0;
	float32_t sumOfmul = 0;
	float32_t covar[LmO];
	
	int32_t error = Sum_Array(Input, LmO, &sum, CMSIS, Order); // LmO = Length minus Order
	if (error < 0) {return -1;}
	
	*mavg = sum/(float32_t)LmO;
	
	for (uint32_t i = 0; i < LmO; i++) {
		//mul = (Input[j] - *mavg);
		covar[i] = Input[j] * Input[j]; //mul * mul;
		j++;
	}
	error = Sum_Array(covar, LmO, &sumOfmul, 0, 0);
	if (error < 0) {return -1;}
  
	float32_t meanOfsquare = sumOfmul/(float32_t)(LmO - 1);
	float32_t squareOfmavg = ((sum * sum)/(float32_t)LmO)/(float32_t)(LmO - 1);
	
	*std = sqrt(meanOfsquare - squareOfmavg);
	
	
	
	return 0;
}

int32_t corr_coeff(float32_t* Input, float32_t* FIR_result, int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* correlation) {
	uint32_t j = (CMSIS == 1) ? Order : 0;
	
	float32_t covar_Sum, std_FIR, std_IN, input_mavg, result_mavg = 0;
	float32_t covar_FIR[LmO], covar_IN[LmO], covar_R[LmO]; // LmO = Length minus Order
		
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

int32_t CMSIS_DSP_lib(float32_t* InputArray, float32_t* FIR_result, float32_t* sub, float32_t* std, float32_t* mean, uint32_t Length){
	arm_sub_f32(InputArray, FIR_result, sub, Length);
	
	arm_std_f32(sub, Length, std);
	
	arm_mean_f32(sub, Length, mean);
	
	return 0;
	
}
int32_t testbench(sub)
{	
	// PART I
	puts("PART I\n");
	srand(time(NULL));

	const uint32_t length = 7;
	const uint32_t order = 5;
	float32_t FIR_coeff[order] = {0.1, 0.15, 0.5, 0.15, 0.1};	
	float32_t InputArray[length] = {0.5, 0.9, 0.34, 0.69, 0.34, 0.12, 0.89};
	float32_t OutputArray_ASM[length];
	float32_t OutputArray_C[length];
	float32_t OutputArray_CMSIS[length];
	
//	for(int32_t i = 0; i < length; i++){
//		InputArray[i] = (float)sin((double)i);
		//printf("InputArray[%d] = %.4f\n", i, InputArray[i]);
//	}
	
	//puts("Modified");
//	for(int32_t j = 0; j < 1; j++){
//		int r = rand() % length;
//		InputArray[r] = InputArray[r] + (float32_t)(rand() % 4);
		//printf("InputArray[%d] = %.4f\n", r, InputArray[r]);
//	}
	
	FIR_ASM(InputArray, OutputArray_ASM, FIR_coeff, length, 4);
		
	FIR_C(InputArray, OutputArray_C, FIR_coeff, length, 4);
	
	FIR_CMSIS(InputArray, OutputArray_CMSIS, FIR_coeff, length, order);
	
	for(int i = 0; i < length; i++){
		printf("OA_C[%d] = %.4f\tOA_ASM[%d] = %.4f\tOA_CMSIS[%d] = %.4f\n", 
			i, OutputArray_C[i], i, OutputArray_ASM[i], i, OutputArray_CMSIS[i]);
	}
	
	//PART II
	puts("\nPART II - A\n");
	// a)
	int32_t size = length-order+1;
	float32_t sub_ASM[size], sub_C[size], sub_CMSIS[size]; 
	int32_t error = Data_Sub(InputArray, OutputArray_ASM, sub_ASM, length, order-1, 0);
	if (error < 0) { printf("Error in data sub assembly"); return -1;}
	
	error = Data_Sub(InputArray, OutputArray_C, sub_C, length, order-1, 0);
	if (error < 0) { printf("Error in data sub C"); return -1;}
	
	error = Data_Sub(InputArray, OutputArray_CMSIS, sub_CMSIS, length, order-1, 1);
	if (error < 0) { printf("Error in data sub CMSIS"); return -1;}
	
	for(int32_t i = 0; i < size; i++){
		printf("Substraction: sub_ASM[%d] = %.4f\tsub_C[%d] = %.4f\tsub_CMSIS[%d] = %.4f\n", 
		i, sub_ASM[i], i, sub_C[i], i, sub_CMSIS[i]);
	}	
	// b)
	float32_t std_ASM, std_C, std_CMSIS;
	float32_t mavg_ASM, mavg_C, mavg_CMSIS;
	//float32_t covar[size];
	error = stdev(sub_ASM, size, 0, 0, &std_ASM, &mavg_ASM);
	if (error < 0) { printf("Error in stdev assembly"); return -1;}
	
	error = stdev(sub_C, size, 0, 0, &std_C, &mavg_C);
	if (error < 0) { printf("Error in stdev C"); return -1;}
	
	error = stdev(sub_CMSIS, size, 0, 0, &std_CMSIS, &mavg_CMSIS);
	if (error < 0) { printf("Error in stdev CMSIS"); return -1;}
	
	printf("Standard deviation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", std_ASM, std_C, std_CMSIS);
	printf("Mean average: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n",mavg_ASM, mavg_C, mavg_CMSIS);
	// c)
	float32_t corr_ASM, corr_C, corr_CMSIS;
	float32_t correlation[size * 2];
	error = corr_coeff(InputArray, OutputArray_ASM, size, 0, 0, &corr_ASM);
	if (error < 0) { printf("Error in correlation assembly"); return -1;}
	error = corr_coeff(InputArray, OutputArray_C, size, 0, 0, &corr_C);
	if (error < 0) { printf("Error in correlation C"); return -1;}
	error = corr_coeff(InputArray, OutputArray_CMSIS, size, 1, order - 1, &corr_CMSIS);
	if (error < 0) { printf("Error in correlation CMSIS"); return -1;}
	printf("Correlation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", corr_ASM, corr_C, corr_CMSIS);
	
	//CMSIS-DSP Library
	puts("\nPART II - B\n");
	float32_t OutputArray_CMSIS_lib[size], sub_ASM_lib[size], sub_C_lib[size], sub_CMSIS_lib[size], std_ASM_lib, std_C_lib, std_CMSIS_lib, mavg_ASM_lib, mavg_C_lib, mavg_CMSIS_lib; 
	
	CMSIS_DSP_lib(InputArray, OutputArray_ASM, sub_ASM_lib, &std_ASM_lib, &mavg_ASM_lib, size);
	CMSIS_DSP_lib(InputArray, OutputArray_C, sub_C_lib, &std_C_lib, &mavg_C_lib, size);
	
	for(int32_t i = 0; i < size; i++){
		OutputArray_CMSIS_lib[i] = OutputArray_CMSIS[i + order - 1];
	}
	CMSIS_DSP_lib(InputArray, OutputArray_CMSIS_lib, sub_CMSIS_lib, &std_CMSIS_lib, &mavg_CMSIS_lib, size);
	
	for(int32_t i = 0; i < size; i++){
		printf("Substraction: sub_ASM_lib[%d] = %.4f\tsub_C_lib[%d] = %.4f\tsub_CMSIS_lib[%d] = %.4f\n", 
		i, sub_ASM_lib[i], i, sub_C_lib[i], i, sub_CMSIS_lib[i]);
	}
		
	printf("Standard deviation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", std_ASM_lib, std_C_lib, std_CMSIS_lib);
	printf("Mean average: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n",mavg_ASM_lib, mavg_C_lib, mavg_CMSIS_lib);
	
	return 0;
}

int main(){
	testbench();
	
	return 0;
}