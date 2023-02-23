
		
		AREA    translate_morse_code, CODE, READONLY

translate_morse FUNCTION         
				
				STMFD SP!, {R4-R8, R10-R11 , LR } ;ABI STD
				
				;R0 -> char* vett_input, Morse code vector to convert.
				;R1 -> int vet_input_lenght, length of the vector to convert.
				;R2 -> char* vett_output, ASCII output vector.
				;R3 -> int vet_output_lenght, length of the output vector.
				
				;general offset => 8 * 4 = 32 byte ABI STD
				
				;char change_symbol, the value representing the change of symbol in the input vector.
				;LDR R4, [SP , #32 ]
				;char space, the value representing the space in the input vector.
				;LDR R5, [SP , #4 + 32]
				; char sentence_end, the value representing the end of the sentence in the input vector.
				;LDR R6, [SP , #8 + 32]
				
				;per ottimizzazione registri multipli accessi allo Stack. Utilizzo unico registro temporaneao
				
				MOV R7, #0 ; number of bit
				MOV R6, #0 ; number of characters
				MOV R10, #0x000000FF ;maschera input value 
				MOV R11, #0 ;numero elementi letti
				
loopInputVect	
				CMP R6, R3
				BGE fineTraduzione ; error the number of charater read is prefixed by EINT1_IRQHandler
				
				LDRB R4, [R0] , #1
				ADD R7, R7, #1
				
checkFineCarattere				
				LDR R5, [SP, #32 ]
				CMP R4, R5 
				BNE CheckSpazio

				;estensione della signature del singolo carattere al numero di bit utilizzati __carattere__ || __bitUtilizzati__
				AND R10, R10, #0x000000FF
				LSL R10, #8
				ORR R10, R10, R11
				STMFD SP!, { R0-R3 }
				MOV R0, R10
				BL searchCarattere
				ADD R6, R6, R0
				LDMFD SP!, { R0-R3 }
				ADD R2, R2, #1
				MOV R10, #0x000000FF ;maschera input value 
				MOV R11, #0 ;numero elementi letti
				B loopInputVect ;nextCarattere

CheckSpazio
				LDR R5, [SP , #36 ]
				CMP R4, R5
				BNE checkFineTraduzione
				
				;estensione della signature del singolo carattere al numero di bit utilizzati __carattere__ || __bitUtilizzati__
				AND R10, R10, #0x000000FF
				LSL R10, #8
				ORR R10, R10, R11
				STMFD SP!, { R0-R3 }
				MOV R0, R10
				BL searchCarattere
				ADD R6, R6, R0
				LDMFD SP!, { R0-R3 }
				ADD R2, R2, #1
				MOV R10, #0x000000FF ;maschera input value 
				MOV R11, #0 ;numero elementi letti
				MOV	R5, #" "
				STRB R5, [R2], #1
				ADD R6, R6, #1
				B loopInputVect ;nextCarattere

checkFineTraduzione
				LDR R5, [SP , #40 ]
				CMP R4, R5
				BNE nextBit
				
				;estensione della signature del singolo carattere al numero di bit utilizzati __carattere__ || __bitUtilizzati__
				AND R10, R10, #0x000000FF
				LSL R10, #8
				ORR R10, R10, R11
				STMFD SP!, { R0-R3 }
				MOV R0, R10
				BL searchCarattere
				ADD R6, R6, R0
				LDMFD SP!, { R0-R3 }
				ADD R2, R2, #1
				MOV R10, #0x000000FF ;maschera input value 
				MOV R11, #0 ;numero elementi letti
				B fineTraduzione ;fineTraduzione
				
nextBit
				CMP R7, R1
				BGE fineTraduzione ;error the last character of sentence must be fineRiga
				
				ADD R11, R11, #1
				LSL R10, #1
				;ORR R4, R4, #0xFFFFFFFE
				ORR R10, R10, R4
				B loopInputVect

			
fineTraduzione		
				MOV R0, R6
				LDMFD SP!, {R4-R8, R10-R11 , LR } ;ABI STD
				
				BX LR
				
                ENDFUNC


searchCarattere FUNCTION
				IMPORT morse_map
				
				STMFD SP!, {R4-R8, R10-R11 , LR } ;ABI STD
				
				;R0 signature completa del codice da tradurre
				
				LDR R5, =morse_map
				MOV R8, #0 ;indice carattere corrispondente
				
linearSearchSignature	
;				LDRB R6, [R5] , #1 ;lunghezza
;				LDRB R7, [R5] , #1 ;carattere
;				CMP R6, R11
;				BNE goNext
;				CMP R7, R10
;				BNE goNext
				
				CMP R8, #30
				BEQ noMatch
				
				LDRH R6, [R5] , #2
				CMP R0, R6
				BEQ directMapAscii
				ADD R8, R8, #1
				B linearSearchSignature

directMapAscii 
				IMPORT ascii_map
				LDR R5, =ascii_map
				LDRB R6, [R5 , R8] 
				STRB R6, [R2] ;write on output vector. Write Back option is demandeted to callee.
				MOV R0, #1 ;return number of characters
				LDMFD SP!, {R4-R8, R10-R11 , PC } ;ABI STD

noMatch
				MOV R0, #0 ;return number of characters
				LDMFD SP!, {R4-R8, R10-R11 , PC } ;ABI STD
				
				ENDFUNC
			
			EXPORT  translate_morse 
	END
		
	
		
	
