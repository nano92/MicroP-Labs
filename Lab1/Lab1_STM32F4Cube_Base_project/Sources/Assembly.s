	AREA subrout, CODE, READONLY
	ENTRY 
	EXPORT FIR_asm
	
FIR_asm	
	PUSH	{R4-R12}
	; OK tenemos un pequeno problema, ya que el loading del float point no se puede hacer con los registers R apparentemente
	; Hay que conseguir una forma de modificar eso
	; Hay que tambien cambiar un poco el agorithmo
	VLDM.f32 R3, {S8-S12}
	VLDR.f32 S0, [R0]		;Load input's last element value
for1
	SUB	  	R0, R0, #4	  	;Points to X[n-1]
	VLDR.f32 S1, [R0]	    ;Loads X[n-1] value
	VMUL.f32 S0, S0, S8 	;Multiplies X[n] with b0
	
	SUB		R0, R0, #4	  	;Points to  X[n-2]
	VMUL.f32 S1, S1, S9 	;Multiplies X[n-1] with b1
	VLDR.f32 S2, [R0]		;Loads X[n-2] value
	VADD.f32 S0, S0, S1 	;Adds X[n-1]*b1 to Sum
	
	SUB		R0, R0, #4	  	;Points to  X[n-3]
	VMUL.f32 S2, S2, S10 	;Multiplies X[n-2] with b2
	VLDR.f32 S1, [R0]		;Loads X[n-3] value
	VADD.f32 S0, S0, S2 	;Adds X[n-2]*b2 to Sum
	
	SUB		R0, R0, #4	  	;Points to  X[n-3]
	VMUL.f32 S1, S1, S11 	;Multiplies X[n-3] with b3
	VLDR.f32 S1, [R0]		;Loads X[n-4] value
	VADD.f32 S0, S0, S1 	;Adds X[n-3]*b3 to Sum
	
	SUB		R0, R0, #4	  	;Points to  X[n-4]
	VMUL.f32 S1, S1, S12 	;Multiplies X[n-4] with b4	
	SUB   	R2, R2, #4		;Decreases the length of for loop	
	VADD.f32 S0, S0, S1 	;Adds X[n-4]*b4 to Sum
	
	VSTR.f32 S0, [R1]    	;Stores the value of the sum into Y[n]
	SUB   	R0, R0, #16 	;Points to X[n]
	ADD   	R1, R1, #4	
	BNE   	R2, #16, for1 	;Check if loop length is Length - Order (It's multiplied by 4 for memory alignment)
	;Probably a stall occurs here
	POP		{R4-R12}
	BX		lr
	END 