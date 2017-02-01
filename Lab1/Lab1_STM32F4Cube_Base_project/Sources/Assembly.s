	AREA subrout, CODE, READONLY
	ENTRY 
	EXPORT FIR_asm
	
FIR_asm	
	PUSH			{R4-R12}
	VPUSH			{S0-S12}
	
	VLDM.f32	R3, {S8-S12}	;Load coefficients 
	SUB 			R2, R2, #4		;Skipped first five elements, Length - Order
	VLDM.f32 	R0, {S1-S5}		;Load first batch of inputs
	ADD 		 	R0, R0, #20		;Points to next input value, after the first filter pass

loop
	;Assuming n = 0
	VMUL.f32 	S0, S1, S8  	;Multiplies X[n] by b0
	VMLA.f32 	S0, S2, S9 		;Multiplies X[n+1] by b1 and adds to the sum
	VMLA.f32 	S0, S3, S10 	;Multiplies X[n+2] by b2 and adds to the sum
	VMOV.f32 	S1, S2       	;Shift X[n+1] to X[n]
	VMLA.f32 	S0, S4, S11  	;Multiplies X[n+3] by b3 and adds to the sum
	VMOV.f32 	S2, S3       	;Shift X[n+2] to X[n+1]
	SUBS 		 	R2, R2, #1		;Generates the exit condition. Set Z(zero) flag high if R2 is zero
	VMOV.f32 	S3, S4				;Shift X[n+3] to X[n+2]
	VMLA.f32 	S0, S5, S12		;Multiplies X[n+4] by b4 and adds to the sum
	VMOV.f32 	S4, S5				;Shift X[n+4] to X[n+3]
	VSTR.f32	S0, [R1]			;Stores value into outputs at index R1
	VLDR.f32 	S5, [R0]			;Load new input at X[n+4]
	ADD				R1, R1, #4		;Points to next output index
	VSUB.f32	S0, S0, S0		;Reset Sum
	ADD 		 	R0, R0, #4		;Points to next input value
	BNE 			loop					;Loop if R2 is not zero
		

	VPOP	{S0-S12}
	POP		{R4-R12}
	BX		lr
	END 