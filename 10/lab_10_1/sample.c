/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT buttons 
 *        	- key1 switches on LED10 
 *				  - key2 switches off all LEDs 
 *			    - int0 switches on LED 11
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 07/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/


#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "RIT/RIT.h"

/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in funct_led								*/
#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
#define MAX_LEN 100

extern char message;
extern char maxLength,fineRiga,spazio,fineCarattere;
extern int translate_morse(char* vett_input, 
													char vet_input_lenght, 
													char* vett_output, 
													char vet_output_lenght, 
													char change_symbol, 
													char space,
													char sentence_end);
													
void init(void);
void run(void);
char count_char(void);

static char countChar = 0;
static char index = 0;
	
int main (void){
  init();
	run();
}

void init(){
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
  BUTTON_init();												/* BUTTON Initialization              */
}

void run(){
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= 0xFFFFFFFFD;						
  while (1) {                           /* Loop forever                       */
		__asm__("wfi");
  }
}

char count_char(){
	
	int8_t tmp = -1 ;
	char* messageAsVector = &(message);
	
	if( index != 0 ) {
			return (char ) -1;
	}
	
	LED_Off_all(); 
	
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
			return countChar ;
		}
	}	
	return (char ) -1;
}

void reset(){
	countChar = 0;
	index = 0;
	LED_On_all();
}

void translate(){
	volatile int RES;
	char vettOutput[MAX_LEN];
	
	if( index == 0 ) {
		return;
	}

	RES = translate_morse(&(message),index, (char* ) vettOutput,countChar,fineCarattere,spazio,fineRiga);
  LED_Out ( RES ) ;
}


void disable_interrupt(){
//During the conversion phase, the buttons INT0 and KEY1 are disabled
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
}

void enable_interrupt(){
	//Buttons INT0 and KEY1 must be enabled, by pressing INT0 the process has to restart from point 1).
	LPC_PINCON->PINSEL4    |= (1 << 22);     /* GPIO pin selection */
	LPC_GPIO2->FIODIR      &= ~(1 << 11);    /* PORT2.11 defined as input          */
	LPC_PINCON->PINSEL4    |= (1 << 20);     /* GPIO pin selection */ 
	LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* PORT2.10 defined as input          */
	NVIC_EnableIRQ(EINT1_IRQn);		/* enable Button interrupts			 */
	NVIC_EnableIRQ(EINT0_IRQn);		/* enable Button interrupts			 */
}



