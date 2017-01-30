#ifndef MAIN_H_   /* Include guard */

#include "arm_math.h"

#define MAIN_H_

float32_t Window_Sum(float32_t* InputArray, float32_t* FIR_coeff, uint32_t Order, uint32_t Location);
int32_t FIR_C(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order);
int32_t FIR_CMSIS(float32_t* InputArray, float32_t* OutputArray, float32_t* FIR_coeff, uint32_t Length, uint32_t Order);

#endif // MAIN_H_