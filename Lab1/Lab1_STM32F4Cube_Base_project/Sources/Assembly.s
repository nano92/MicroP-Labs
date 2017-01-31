	AREA subrout, CODE, READONLY
	ENTRY 
	EXPORT FIR_asm
	
FIR_asm	
	PUSH	{R4-R12}
	LDM 	R3, {R8-R12}
	
	;En vez de multiplicar por 4 debemos hacer un shift, es mas rapido
	
	MOV		R3, #4
	MUL		R2, R2, R3	;Length of for loop in C needs to be multiplied by 4, for memory alignment
	SUB		R2, R2, #4	;Index of last input element
	MUL   R1, R1, R3	;Calculate offset for index of output's last element
	SUB   R1, R1, #24 ;Index of last output element
	ADD		R0, R0, R2	;Input's last element address
	LDR		R3, [R0]		;Load input's last element value
for1
	SUB		R0, R0, #4	  ;Points to X[n-1]
	LDR	  R4, [R0]	    ;Loads X[n-1] value
	VMUL.f32 R3, R3, R8 ;Multiplies X[n] with b0
	SUB		R0, R0, #4	  ;Points to  X[n-2]
	VMUL.f32 R4, R4, R9 ;Multiplies X[n-1] with b1
	
	VADD.f32 R3, R3, R4 ;Adds X[n-1]*b1 to Sum
	LDR	  R4, [R0]			;Loads X[n-2] value
	SUB		R0, R0, #4	  ;Points to  X[n-3]
	VMUL.f32 R4, R4, R10 ;Multiplies X[n-2] with b2

	VADD.f32 R3, R3, R4 ;Adds X[n-2]*b2 to Sum
	LDR	  R4, [R0]			;Loads X[n-3] value
	SUB		R0, R0, #4	  ;Points to  X[n-3]
	VMUL.f32 R4, R4, R11 ;Multiplies X[n-3] with b3

	VADD.f32 R3, R3, R4 ;Adds X[n-3]*b3 to Sum
	LDR	  R4, [R0]			;Loads X[n-4] value
	SUB		R0, R0, #4	  ;Points to  X[n-4]
	VMUL.f32 R4, R4, R12 ;Multiplies X[n-4] with b4
	
	SUB   R2, R2, #4	;Decrease length of for loop
	
	VADD.f32 R3, R3, R4 ;Adds X[n-4]*b4 to Sum
	
	
	STR   R3, [R1]    ;Stores the value of the sum into Y[n]
	ADD   R0, R0, #16 ;Points to X[n]
	SUB   R1, R1, #4
	
	BNE   R2, #16, for1 ;Check if loop length is Length - Order (It's multiplied by 4 for memory alignment)
	
	POP		{R4-R12}
	BX		lr
	END 