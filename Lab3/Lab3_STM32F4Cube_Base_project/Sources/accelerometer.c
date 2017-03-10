
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "lis3dsh.h"
#include "accelerometer.h"
#include <math.h>

float ACCX_1[3] = {-0.00096545, -2.56184E-05,	8.70825E-06};
float ACCX_2[3] = {0.0000223103,	0.001024109,	-0.0000556414};
float ACCX_3[3] = {-0.0000130817,	-0.00000547623,	0.000979288};
float ACCX_0[3] = {0.000849161,	-0.003375861,	-0.036292987};

float COEFF[7] = {0.083333333,	0.125,	0.166666667,	0.25,	0.166666667,	0.125,	0.083333333};
float X_MEM[7] = {0,0,0,0,0,0,0};
float Y_MEM[7] = {0,0,0,0,0,0,0};
float Z_MEM[7] = {0,0,0,0,0,0,0};

float ROLL;

/* Function : calibrate
 * Input    : float* acc
 * Description: Calibrates the values obtained from the accelerometer.
*/
void calibrate(float* acc) {
	float tempx,tempy,tempz;
	tempx = acc[0];
	tempy = acc[1];
	tempz = acc[2];
	
	acc[0] = tempx*ACCX_1[0] + tempy*ACCX_2[0] + tempz*ACCX_3[0] + ACCX_0[0];
	acc[1] = tempx*ACCX_1[1] + tempy*ACCX_2[1] + tempz*ACCX_3[1] + ACCX_0[1];
	acc[2] = tempx*ACCX_1[2] + tempy*ACCX_2[2] + tempz*ACCX_3[2] + ACCX_0[2];
}
/* Function : filter
 * Input    : float currentValue, float* prevValues
 * Returns  : float filteredValue
 * Description: Outputs a filtered value from the previous 6 signal values passed through this system.
*/
float filter(float currentValue, float* prevValues) {
	float filteredValue = 0.0;
	for(int8_t i = 5; i >= 0; i--) {
		prevValues[i+1] = prevValues[i];
	}
	prevValues[0] = currentValue;
	for(int8_t i = 0; i < 7; i++) {
		filteredValue = prevValues[i]*COEFF[i] + filteredValue;
	}
	return filteredValue
}
/* Function : getPitch
 * Input    : float x, float y, float z
 * Returns  : float pitch
 * Description: Calculates the pitch angle on the board based on the positions obtained from the filtered accelerometer values.
*/
float getPitch (float x, float y, float z) {
	float pitch = atan2(x, (sqrt(y*y+z*z))) * 180.0 / PI;
	
	if ( z < 0 && pitch < 0){
		pitch = 180 + pitch;
	}
	else if( z < 0 && pitch > 0) {
			pitch = 180 - pitch;
	}
	else if ( pitch < 0 ){
		pitch = -pitch;
	}
	return pitch;
}

/* Function : getRoll
 * Input    : float x, float y, float z
 * Returns  : float roll
 * Description: Calculates the roll angle on the board based on the positions obtained from the filtered accelerometer values.
*/
float getRoll (float x, float y, float z) {
	float roll = atan2(y, (sqrt(x*x+z*z))) * 180.0 / PI;
	
	if ( z < 0 && roll < 0){
		roll = 180 + roll;
	}
	else if( z < 0 && roll > 0) {
			roll = 180 - roll;
	}
	else if ( roll < 0 ){
		roll = -roll;
	}
	return roll;
}

void readingACC() {
	float acc[3];
	LIS3DSH_ReadACC(acc);
	calibrate(acc);
	
	float acc_x = filter(acc[0],X_MEM);
	float acc_y = filter(acc[1],Y_MEM);
	float acc_z = filter(acc[2],Z_MEM);
	
	ROLL = getRoll(&acc_x, &acc_y, &acc_z);
}
