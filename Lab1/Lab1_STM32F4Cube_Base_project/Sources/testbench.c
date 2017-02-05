#include "main.h"


/* Function   : testbench
 * Description: Runs all the required tasks for part I and part II of lab1.
 */
int32_t testbench()
{	
	// PART I
	puts("PART I\n");
	srand(time(NULL));

	const uint32_t length = 20;
	const uint32_t order = 5;
	float32_t FIR_coeff[order] = {0.1, 0.15, 0.5, 0.15, 0.1};	
	float32_t InputArray[length] = {1.0, 1.07, 1.15, 1.2, 1.25, 1.3, 1.358, 1.39, 1.15, 1.2, 1.15, 1.1, 1.05, 1.0, 0.8, 0.6, 0.4, 0.0, -0.3, -0.8};//{0.5, 0.9, 0.34, 0.69, 0.34, 0.12, 0.89, 0.43};
	float32_t OutputArray_ASM[length];
	float32_t OutputArray_C[length];
	float32_t OutputArray_CMSIS[length];

	FIR_asm(InputArray, OutputArray_ASM, length, FIR_coeff);
		
	FIR_C(InputArray, FIR_coeff, length, order - 1, OutputArray_C);
	
	FIR_CMSIS(InputArray, OutputArray_CMSIS, FIR_coeff, length, order);
	
	// Prints out the values obtained from each filter implementation
	for(int i = 0; i < length; i++){
		printf("OA_C[%d] = %.4f\tOA_ASM[%d] = %.4f\tOA_CMSIS[%d] = %.4f\n", 
			i, OutputArray_C[i], i, OutputArray_ASM[i], i, OutputArray_CMSIS[i]);
	}
	
	//PART II
	puts("\nPART II - A\n");

	// a) Get the difference between the input and the filtered value
	const int32_t size = length-order+1;
	float32_t sub_ASM[size], sub_C[size], sub_CMSIS[size]; 
	
	// Difference with respect to the assembly based FIR filter and the input
	int32_t error = Data_Sub(InputArray, OutputArray_ASM, length, order-1, 0, sub_ASM);
	if (error < 0) { printf("Error in data sub assembly"); return -1;}
	
	// Difference with respect to the C based FIR filter and the input
	error = Data_Sub(InputArray, OutputArray_C, length, order-1, 0, sub_C);
	if (error < 0) { printf("Error in data sub C"); return -1;}
	
	// Difference with respect to the CMSIS FIR filter and the input
	error = Data_Sub(InputArray, OutputArray_CMSIS, length, order-1, 1, sub_CMSIS);
	if (error < 0) { printf("Error in data sub CMSIS"); return -1;}
	
	// Prints out the results obtained from the 3 substraction processes
	for(int32_t i = 0; i < size; i++){
		printf("Substraction: sub_ASM[%d] = %.4f\tsub_C[%d] = %.4f\tsub_CMSIS[%d] = %.4f\n", 
		i, sub_ASM[i], i, sub_C[i], i, sub_CMSIS[i]);
	}	
	
	// b) Get the standard deviation and average for each difference
	float32_t std_ASM, std_C, std_CMSIS;
	float32_t mavg_ASM, mavg_C, mavg_CMSIS;
	//Temporary variable used in the standard deviation function and in the correlation coefficient fucntion
	float32_t temp_array[size];
	
	// Calculating the standard deviation and average for the assembly based difference
	error = stdev(sub_ASM, size, 0, 0, temp_array, &std_ASM, &mavg_ASM);
	if (error < 0) { printf("Error in stdev assembly"); return -1;}
	
	// Calculating the standard deviation and average for the C based difference
	error = stdev(sub_C, size, 0, 0, temp_array, &std_C, &mavg_C);
	if (error < 0) { printf("Error in stdev C"); return -1;}
	
	// Calculating the standard deviation and average for the CMSIS FIR difference
	error = stdev(sub_CMSIS, size, 0, 0, temp_array, &std_CMSIS, &mavg_CMSIS);
	if (error < 0) { printf("Error in stdev CMSIS"); return -1;}
	
	// Prints the results obtained for each standard deviation and average
	printf("Standard deviation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", std_ASM, std_C, std_CMSIS);
	printf("Mean average: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n",mavg_ASM, mavg_C, mavg_CMSIS);
	
	// c) Calculate the correlation between the input and the different FIR filters type
	float32_t corr_ASM, corr_C, corr_CMSIS;
	
	// Calculate the correlation between the input and assembly based FIR filter's output values
	error = corr_coeff(InputArray, OutputArray_ASM, size, 0, 0, temp_array, &corr_ASM);
	if (error < 0) { printf("Error in correlation assembly"); return -1;}
	
	// Calculate the correlation between the input and C based FIR filter's output values
	error = corr_coeff(InputArray, OutputArray_C, size, 0, 0, temp_array, &corr_C);
	if (error < 0) { printf("Error in correlation C"); return -1;}
	
	// Calculate the correlation between the input and CMSIS FIR filter's output values
	error = corr_coeff(InputArray, OutputArray_CMSIS, size, 1, order - 1, temp_array, &corr_CMSIS);
	if (error < 0) { printf("Error in correlation CMSIS"); return -1;}
	
	// Prints the results obtained for each correlation 
	printf("Correlation: ASM = %.4f\tC = %.4f\tCMSIS = %.4f\n", corr_ASM, corr_C, corr_CMSIS);
	
	// Repeat of PART II by using the CMSIS-DSP Library 
	puts("\nPART II - B\n");

	float32_t OutputArray_CMSIS_lib[size], sub_ASM_lib[size], sub_C_lib[size], sub_CMSIS_lib[size], corr_ASM_lib[size], corr_C_lib[size];
  float32_t	corr_CMSIS_lib[size], std_ASM_lib, std_C_lib, std_CMSIS_lib, mavg_ASM_lib, mavg_C_lib, mavg_CMSIS_lib; 
	
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
	
	for(int32_t i = 0; i < size; i++){
		printf("Correlation: corr_ASM_lib[%d] = %.4f\tcorr_C_lib[%d] = %.4f\tcorr_CMSIS_lib[%d] = %.4f\n", 
		i, corr_ASM_lib[i], i, corr_C_lib[i], i, corr_CMSIS_lib[i]);
	}
	
	return 0;
}