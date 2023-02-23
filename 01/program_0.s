#Nota: notazione CA2 su 8 bit -> -128 <= (N)[CA2] <= 127.
#I numeri scelti per riempire i vettori sono forniti in modo da non ottenere OVERFLOW su 8 bit. 

.data
	a: .byte -32,41,29,-51,-44,53,54,4,-7,-27,2,61,44,-36,-33,36,-40,-35,62,-13,43,8,-12,-18,13,40,16,-16,-10,-30,12,38,-52,24,20,9,-28,1,-24,42
	b: .byte 21,-41,28,-16,46,41,61,-63,-54,-62,-24,26,-15,18,20,37,-32,35,33,13,8,-36,27,-60,-46,-38,32,22,31,-33,42,-13,-7,3,-26,-10,-29,-25,6,48
	c: .space 40
	max: .space 1
	min: .space 1	
	
.text
main: 
	daddu R1,  R0,  R0 #R1 viene utilizzato sia come contatore che come spiazzamento per accedere ad i singoli byte dei vettori
	daddui R2,  R0,  40
	
#vettore somma c[i] = a[i] + b[i]
for0:
	lb R4,  a(R1) #a[i]
	lb R5,  b(R1) #b[i]
	dadd R4,  R4,  R5
	sb R4,  c(R1)
	daddui R1,  R1,  1 #incrementa il contatore di 1
	bne R1,  R2,  for0

#min(c) , max(c)
	daddu R1,  R0,  R0
	lb R3, c(R1) #massimo
	dadd R4, R0, R3 #minimo
for1:
	lb R5, c(R1) # c[i]
	slt R6,  R5,  R3 # R6=1 se c[i] < MASSIMO
	slt R7,  R4,  R5 # R7=1 se minimo < c[i]
	bne R6, R0, nextMax
	daddu R3,  R0,  R5 #set new MAX
nextMax: 
		bne R7, R0, nextMin
		daddu R4,  R0,  R5 #set new MIN
nextMin:
		daddui R1,  R1,  1 
	bne R1,  R2,  for1
	sb R3,  max(R0)
	sb R4,  min(R0)
	HALT