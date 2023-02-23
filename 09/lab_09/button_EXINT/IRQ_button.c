#include "button.h"
#include "lpc17xx.h"
#include <stdlib.h>

#include "../led/led.h"

#define MAX_LEN 100

extern char maxLength,fineRiga,spazio,fineCarattere;
extern char message;
extern void run(void);

static char countChar = 0;
static char index = 0 ;

extern int translate_morse(char* vett_input, 
													char vet_input_lenght, 
													char* vett_output, 
													char vet_output_lenght, 
													char change_symbol, 
													char space,
													char sentence_end);
	

void EINT0_IRQHandler (void)	  
{
  countChar = 0;
	index = 0;
	LED_On_all();
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}

void EINT1_IRQHandler (void)	  
{
	int8_t tmp = -1 ;
	char* messageAsVector = &(message);
	
	if( index != 0 ) {
			return;
	}
	
	LED_Off_all(); //All LEDs are switched off
	
	while( index <  maxLength ) {
		tmp = messageAsVector[index];
		index +=1 ;
		if( tmp == fineCarattere  ){
			countChar += 1;
			LED_Out(countChar);
		}else if( tmp == spazio ) {
			countChar += 1;
			LED_Out(countChar);
			countChar += 1; //The space has to be considered as a converted symbol
		}else if( tmp == fineRiga ){
			countChar += 1;
			LED_On_all(); //At the end of the message, all LEDs are switched on, independently from the number of read symbols.			
			LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
			return;
		}
	}	
}

void EINT2_IRQHandler (void)	  
{
	volatile int RES;
	char vettOutput[MAX_LEN];

	if( index == 0 ) {
		return;
	}
	
	//During the conversion phase, the buttons INT0 and KEY1 are disabled
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	
	RES = translate_morse(&(message),index, (char* ) vettOutput,countChar,fineCarattere,spazio,fineRiga);
  LED_Out ( RES ) ;
	
	//Buttons INT0 and KEY1 must be enabled, by pressing INT0 the process has to restart from point 1).
	LPC_PINCON->PINSEL4    |= (1 << 22);     /* GPIO pin selection */
	LPC_GPIO2->FIODIR      &= ~(1 << 11);    /* PORT2.11 defined as input          */
	LPC_PINCON->PINSEL4    |= (1 << 20);     /* GPIO pin selection */ 
	LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* PORT2.10 defined as input          */
	NVIC_EnableIRQ(EINT1_IRQn);		/* enable Button interrupts			 */
	NVIC_EnableIRQ(EINT0_IRQn);		/* enable Button interrupts			 */
	
	LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */ 
}


