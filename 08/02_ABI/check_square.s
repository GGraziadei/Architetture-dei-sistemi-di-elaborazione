
		AREA    |check_square.text|, CODE, READONLY

; check_square 
; parameters R0 = x , R1 = y , R2 = RADIUS
; return value in R0, 1 if inside 0 if outside

check_square	FUNCTION                     
				PUSH {R4-R6,R10-R11,LR}
				MUL R4, R0, R0
				MUL R5, R1, R1
				MUL R6, R2, R2
				ADD R4, R4, R5
				
				CMP R4, R6
				
				MOVLE R0, #1
				MOVGT R0, #0
				
				POP {R4-R6,R10-R11,PC}
				ENDFUNC
		
		EXPORT check_square
			
		END
		
		

		