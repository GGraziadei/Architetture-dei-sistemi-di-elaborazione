/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern void reset(void);
extern char count_char(void);
extern void enable_interrupt(void);
extern void disable_interrupt(void);
extern void translate(void);

void RIT_IRQHandler (void)
{			
	static int down[4] = { 0 };	//map with buttons

	//EINT0
	if((LPC_PINCON->PINSEL4 & (1 << 20)) == 0){
		down[0]++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){   
			reset_RIT();
			
			//My FUNCTION 			
			reset(); 
		}
		else {	/* button released */
			down[0]=0;			
			disable_RIT();
			reset_RIT();
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	//EINT1
	else if((LPC_PINCON->PINSEL4 & (1 << 22)) == 0){
		down[1]++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){   
			reset_RIT();
			
			//My FUNCTION 
			count_char();
		}
		else {	/* button released */
			down[1]=0;			
			disable_RIT();
			reset_RIT();
			NVIC_EnableIRQ(EINT1_IRQn);							 
			LPC_PINCON->PINSEL4    |= (1 << 22);     
		}
	}
	
	//EINT2
	else if((LPC_PINCON->PINSEL4 & (1 << 24)) == 0){
		down[2]++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){   
			reset_RIT();
			
			//My FUNCTION 
			disable_interrupt();
			translate();
			enable_interrupt();
		}
		else {	/* button released */
			down[2]=0;			
			disable_RIT();
			reset_RIT();
			NVIC_EnableIRQ(EINT2_IRQn);							 
			LPC_PINCON->PINSEL4    |= (1 << 24);     
		}
	}

  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
