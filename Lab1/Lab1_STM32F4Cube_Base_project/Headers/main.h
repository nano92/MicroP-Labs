#ifndef MAIN_H_   /* Include guard */

#include <stdio.h>
#include <time.h>
#include "arm_math.h"

#define MAIN_H_

extern void FIR_asm(float32_t* InputArray, float32_t* OutputArray, int32_t Length, float32_t* FIR_coeff);

int32_t FIR_C(float32_t* InputArray, float32_t* FIR_coeff, int32_t Length, int32_t Order, float32_t* OutputArray);
int32_t FIR_CMSIS(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order);
int32_t Data_Sub(float32_t* InputArray, float32_t* FilterArray, uint32_t Length, uint32_t Order, uint16_t CMSIS, float32_t* ResultArray);
int32_t Sum_Array(float32_t* A, uint32_t Length, float32_t* sum, uint16_t CMSIS, uint32_t Order);
int32_t stdev(float32_t* Input, const int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* covar, float32_t* std,  float32_t* mavg);
int32_t corr_coeff(float32_t* Input, float32_t* FIR_result, const int32_t LmO, uint16_t CMSIS, uint32_t Order, float32_t* covar, float32_t* correlation);
int32_t CMSIS_DSP_lib(float32_t* InputArray, float32_t* FIR_result, uint32_t Length, float32_t* sub, float32_t* std, float32_t* mean, float32_t* Corr_Array);
int32_t testbench();

#endif // MAIN_H_