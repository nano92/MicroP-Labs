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
	
	VPOP     {S0-S12}
	POP		{R4-R12}
	BX		lr
	END 