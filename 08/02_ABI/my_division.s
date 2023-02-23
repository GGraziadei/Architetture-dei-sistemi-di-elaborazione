
		AREA    |my_division.text|, CODE, READONLY

; my_division 
; parameters R0 = a , R1 = b
; return value in R0

my_division	FUNCTION  
	
				PUSH {R4-R6,R10-R11,LR}
				PUSH {R0-R3}
				
				IMPORT __aeabi_fdiv
				LDR	 R4, =__aeabi_fdiv
				
				LDR  R5, [R0]
				LDR  R6, [R1]
				
				MOV	 R0, R5
				MOV  R1, R6
				BX	 R4
				MOV  R4, R0
	
				POP  {R0-R3}
				MOV  R0, R4
				POP  {R4-R6,R10-R11,PC}
				ENDFUNC
		
		EXPORT my_division
			
		END
		
		

		