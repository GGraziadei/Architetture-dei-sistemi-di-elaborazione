#include "button.h"
#include "lpc17xx.h"
#include <stdlib.h>

#include "../led/led.h"
#include "../RIT/RIT.h"	



void EINT0_IRQHandler (void)	  
{
	enable_RIT();															/* enable RIT to count 50ms				 */
	NVIC_DisableIRQ(EINT0_IRQn);							/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}

void EINT1_IRQHandler (void)	  
{
	enable_RIT();
	NVIC_DisableIRQ(EINT1_IRQn);							/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  
{
	enable_RIT();
	NVIC_DisableIRQ(EINT2_IRQn);							/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	
	LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */ 
}


