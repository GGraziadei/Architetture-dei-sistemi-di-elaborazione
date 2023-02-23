.data
	v1:  		.byte 	1,2,3,4,5,6,7,100,7,6,5,4,3,2,1
	palindrome: .space 	1
	
.text
main:
	daddu  R1, R0, R0
	daddui R2, R0, 14
	daddui R3, R0, 7 
	daddui R4, R0, 1 #isPalindrome
	
for0:
	lb 	R5, v1(R1)
	lb 	R6, v1(R2)
	bne R6, R5, isNotPalindrome
	daddui 	R1, R1, 1 #incrementa il contatore di 1
	dsubu 	R2, R2, R4 	
	bne 	R1,  R3,  for0
	sb 		R4, palindrome(R0)
	HALT
isNotPalindrome: 
	daddu  R4, R0, R0
	sb R4, palindrome(R0)
	HALT