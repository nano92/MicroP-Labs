	AREA subrout, CODE, READONLY
	ENTRY 
	EXPORT FIR_asm
	
FIR_asm	
	PUSH	{R4-R12}
	VPUSH    {S0-S12}
	; OK tenemos un pequeno problema, ya que el loading del float point no se puede hacer con los registers R apparentemente
	; Hay que conseguir una forma de modificar eso
	; Hay que tambien cambiar un poco el agorithmo
	ADD R0, R0, #16    ; Starts the filtering at the Order-th input
	VLDM.f32 R3, {S8-S12}
	SUB R2, R2, #4
	VLDM.f32 R0, {S1-S5}	;Load fisrt batch of inputs
fdat
	VMUL.f32 S0, S5, S12    ;Multiplies X[n-4] by b4
	VMLA.f32 S0, S4, S11 	;Multiplies X[n-3] by b3 and adds to the sum
	VMOV.f32 S5, S4         ;Shift X[n-3] to X[n-4]
	VMLA.f32 S0, S3, S10 	;Multiplies X[n-2] by b2 and adds to the sum
	VMOV.f32 S4, S3         ;Shift X[n-2] to X[n-3]
	VMLA.f32 S0, S2, S9  	;Multiplies X[n-1] by b1 and adds to the sum
	ADD R0, R0, #4			;Points to next input value
	VMOV.f32 S3, S2			;Shift X[n-1] to X[n-2]
	VMLA.f32 S0, S1, S8		;Multiplies X[n] by b0 and adds to the sum
	SUB R2, R2, #1			;Generates the exit condition
	VMOV.f32 S2, S1			;Shift X[n] to X[n-1]
	VLDR.f32 S1, [R0]		;Load new input
	CMP R2, #0
	BEQ fdat

;####################################################################
;	SUB	  	R0, R0, #4	  	;Points to X[n-1]
;	VLDR.f32 S1, [R0]	    ;Loads X[n-1] value
;	VMUL.f32 S0, S0, S8 	;Multiplies X[n] with b0
;	
;	SUB		R0, R0, #4	  	;Points to  X[n-2]
;	VMUL.f32 S1, S1, S9 	;Multiplies X[n-1] with b1
;	VLDR.f32 S2, [R0]		;Loads X[n-2] value
;	VADD.f32 S0, S0, S1 	;Adds X[n-1]*b1 to Sum
;	
;	SUB		R0, R0, #4	  	;Points to  X[n-3]
;	VMUL.f32 S2, S2, S10 	;Multiplies X[n-2] with b2
;	VLDR.f32 S1, [R0]		;Loads X[n-3] value
;	VADD.f32 S0, S0, S2 	;Adds X[n-2]*b2 to Sum
;	
;	SUB		R0, R0, #4	  	;Points to  X[n-3]
;	VMUL.f32 S1, S1, S11 	;Multiplies X[n-3] with b3
;	VLDR.f32 S1, [R0]		;Loads X[n-4] value
;	VADD.f32 S0, S0, S1 	;Adds X[n-3]*b3 to Sum
;	
;	SUB		R0, R0, #4	  	;Points to  X[n-4]
;	VMUL.f32 S1, S1, S12 	;Multiplies X[n-4] with b4	
;	SUB   	R2, R2, #4		;Decreases the length of for loop	
;	VADD.f32 S0, S0, S1 	;Adds X[n-4]*b4 to Sum
;	
;	VSTR.f32 S0, [R1]    	;Stores the value of the sum into Y[n]
;	ADD   	R0, R0, #20 	;Points to X[n]
;	ADD   	R1, R1, #4	
;	BNE   	for1 	        ;Check if loop length is Length - Order (It's multiplied by 4 for memory alignment)
	;Probably a stall occurs here
	VPOP     {S0-S12}
	POP		{R4-R12}
	BX		lr
	END 