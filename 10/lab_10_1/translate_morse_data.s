		AREA    translate_ascii_map, DATA, READONLY, ALIGN=1

ascii_map 
			DCB 'A','B','C','D','E','F','G','H','I'
			DCB 'J','K','L','M','N','O','P','Q','R'
			DCB 'S','T','U','V','W','X','Y','Z'
			DCB '1','2','3','4','5','6','7','8','9','0'
					
			
morse_map 
			DCB 2, 2_11111101  ;0,1     ;A
			DCB 4, 2_11111000  ;1,0,0,0 ;B
			DCB 4, 2_11111010  ;1,0,1,0 ;c
			DCB 3, 2_11111100  ;1,0,0	  ;D
			DCB 1, 2_11111110  ;0 	  ;E
			DCB	4, 2_11110010  ;0,0,1,0 ;F
			DCB 3, 2_11111110  ;1,1,0 ;G
			DCB 4, 2_11110000  ;0,0,0,0 ;H
			DCB 2, 2_11111100  ;0,0	  ;I
			DCB 4, 2_11110111  ;0,1,1,1 ;J
			DCB 3, 2_11111101  ;1,0,1	  ;K
			DCB 4, 2_11110100 ;0,1,0,0 ;L
			DCB 2, 2_11111111 ;1,1	  ;M
			DCB 2, 2_11111110 ;1,0     ;N
			DCB 3, 2_11111111 ;1,1,1   ;O
			DCB 4, 2_11110110 ;0,1,1,0 ;P
			DCB 4, 2_11111101 ;1,1,0,1 ;Q
			DCB 3, 2_11111010 ;0,1,0   ;R
			DCB 3, 2_11111000 ;0,0,0   ;S
			DCB 1, 2_11111111 ;1		  ;T
			DCB 3, 2_11111001 ;0,0,1   ;U
			DCB 4, 2_11110001 ;0,0,0,1 ;V
			DCB 3, 2_11111011 ;0,1,1	  ;W
			DCB 4, 2_11111001 ;1,0,0,1 ;X
			DCB 4, 2_11111011 ;1,0,1,1 ;Y
			DCB 4, 2_11111100 ;1,1,0,0 ;Z
			
			DCB 5, 2_11101111 ;0,1,1,1,1 ;1
			DCB 5, 2_11100111 ;0,0,1,1,1 ;2
			DCB 5, 2_11100011 ;0,0,0,1,1 ;3
			DCB 5, 2_11100001 ;0,0,0,0,1 ;4
			DCB 5, 2_11100000 ;0,0,0,0,0 ;5
			DCB 5, 2_11110000 ;1,0,0,0,0 ;6
			DCB 5, 2_11111000 ;1,1,0,0,0 ;7
			DCB 5, 2_11111100 ;1,1,1,0,0 ;8
			DCB 5, 2_11111110 ;1,1,1,1,0 ;9
			DCB 5, 2_11111111 ;1,1,1,1,1 ;0
			
			EXPORT ascii_map
			EXPORT morse_map
					
	END	