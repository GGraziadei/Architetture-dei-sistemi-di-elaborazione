	AREA    |svc_custom_handlers.text|, CODE, READONLY

SVC_Handler     PROC
                EXPORT  SVC_Handler               

				PUSH  {R0-R12 , LR} 
				
				MRS R5, PSP
				LDR	R4, [ R5, #24]
				SUB R7, R4, #4 ; original PC 
				LDR R4, [R7]
				BIC R4, #0xFF000000
				LSR R4, #16 ;R4 contains the comment SVC code

svc0xca
				CMP R4, #0xCA ; LR = 0xFFFFFFFD
				BNE svc0xfe
				
				POP {R0-R12 , LR}
				
				EOR R0, R0, R1
				EOR R0, R0, R2
				EOR R0, R0, R3
				EOR R0, R0, R4
				EOR R0, R0, R5
				EOR R0, R0, R6
				EOR R0, R0, R7
				EOR R0, R0, R8
				EOR R0, R0, R9
				EOR R0, R0, R10
				EOR R0, R0, R11
				EOR R0, R0, R12
				EOR R0, R0, LR
				
				MRS R6, xPSR
				EOR R0, R0, R6
				MRS R5, PSP
				STMFD R5, {R0} 
				
				BX LR

svc0xfe			
				CMP R4, #0xFE
				BNE .
				
				IMPORT Opt_M_Coordinates
				IMPORT Matrix_Coordinates
				IMPORT ROWS
				IMPORT COLUMNS
				
				LDRB R0, ROWS
				LDRB R1, COLUMNS
				MUL R0, R1, R0
				
				LDR R1, =Matrix_Coordinates
				LDR R2, =Opt_M_Coordinates

				MOV R3, #0
loop
				LDR  R4, [R1], #4
				STRB R4, [R2], #1	
				ADD R3, R3, #1
				CMP R3, R0
				BNE loop
				
				POP {R0-R12 , LR}
				BX LR
				
                ENDP
					
	END