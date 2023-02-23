	AREA input_data, DATA, READONLY, ALIGN=1

message	DCB 0,0,0,0,2
		DCB 1,1,1,2
		DCB 0,1,0,0,2
		DCB 0,1,3
		DCB 1,1,2
		DCB 0,0,1,2
		DCB 1,0,2
		DCB 1,0,0,2
		DCB 1,1,1,3
		DCB 0,1,1,1,1,2
		DCB 0,0,1,1,1,4

maxLength DCB 100

fineRiga DCB 4

spazio DCB 3

fineCarattere DCB 2
		
	EXPORT message
	EXPORT maxLength
	EXPORT fineRiga
	EXPORT spazio
	EXPORT fineCarattere
	
	END