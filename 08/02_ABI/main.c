#define RADIUS 5

extern int   Matrix_Coordinates;  //matrice di elementi formati da 2B
extern char  ROWS;								//puntatore a byte
extern char  COLUMNS;							//puntatore a byte

extern int check_square(int x, int y, int r);
extern float my_division(float* a, float* b);

int main(void){
	
	int i,x,y,dim = ROWS * COLUMNS ;
	int* matrix =   &Matrix_Coordinates;
	float radiusCast = (float) ( RADIUS * RADIUS );
	volatile float area=0.;
	volatile float pi=0.;
	
	for( i = 0; i < dim ; i +=2 ) 
	{
		x = matrix[i];
		y = matrix[i + 1];
		area += check_square(x,y,RADIUS);
	}
	
	pi = my_division( (float * ) &area, &radiusCast );
	

	__asm__("svc 0xca"); //check program signature 
	
	__asm__("svc 0xfe"); //memory compaction
	
}
