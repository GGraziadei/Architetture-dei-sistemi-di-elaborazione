/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"

#include "tamagotchi.h"
#include "user_events.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

void init(void);
void run (void);

static session_t session ;
static events_buffer_t events_buffer_staging;
static events_buffer_t events_buffer_done;

void do_tamagotchi_life_interrupt( void );
void do_user_events_interrupt( void );
void do_tamagotchi_update(void);

	
int main(void)
{
	init();
	run();
}

void run(){
	session_play( &session );
	user_events_init();
	while (1)	
  {
		__asm__("wfi");
  }
}

void init(){

	SystemInit();  												/* System Initialization  */
	session = session_init();
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);	
	
	events_buffer_staging = events_buffer_init();
	events_buffer_done = events_buffer_init();
	
}

void do_tamagotchi_life_interrupt(){
	tamagotchi_life_interrupt ( &session ) ;
}

void do_tamagotchi_update(){
	tamagotchi_eat_interrupt( &session ) ;
}

void do_user_events_interrupt(){
	
	int new_event = 1;
	
	user_event_t event_tmp;
	user_event_t* event_p;
	event_tmp.trigger_number = 1;
	event_tmp.event = VOID_EVENT;
		
	if( ( LPC_GPIO1->FIOPIN & ( 1<<25 )) == 0 ) {
		//SEL PRESSED -- INIT status for new session
		if ( session.game_status_var == INIT && session.jc == RESET_JC) {
			event_tmp.event = CLK_BTN_RESET;
		}
		if( session.game_status_var == PLAY && session.jc == SNACK_JC){
			event_tmp.event = CLK_BTN_SNACK;
		}
		if( session.game_status_var == PLAY && session.jc == MEAL_JC){
			event_tmp.event = CLK_BTN_MEAL;
		}
	}else if ( ( LPC_GPIO1->FIOPIN & ( 1<<27 )) == 0 ){
		//LEFT PRESSED
		event_tmp.event = SELECT_MEAL;
	}else if( ( LPC_GPIO1->FIOPIN & ( 1<<28 )) == 0 ){
		//RIGHT PRESSED
		event_tmp.event = SELECT_SNACK;
	}
	 
	//Serializzazione eventi
	while ( events_buffer_staging.size > 0 ) {
		event_p = pop(&events_buffer_staging);
		if( event_p->event == event_tmp.event ) {
			new_event = 0;
			event_p->trigger_number += 1; //Track the number of triggers that this event catches
			push( &events_buffer_staging , *event_p );
			break;
		} else {
			push( &events_buffer_done , *event_p ); //Go to DONE area ( serialization completed )
		}
	}
	
	if( event_tmp.event != VOID_EVENT && new_event ) {
		push( &events_buffer_staging , event_tmp );
	}
	
	while ( events_buffer_done.size > 0 ) {
		event_p = pop(&events_buffer_done);
		switch ( event_p->event ) {
			case SELECT_SNACK : jc_selection( &session , SNACK_JC);  break;
			case SELECT_MEAL : jc_selection( &session , MEAL_JC); break;
			case CLK_BTN_SNACK : tamagotchi_user_wrapper_update(&session , event_p->event ); break;
			case CLK_BTN_MEAL : tamagotchi_user_wrapper_update(&session , event_p->event); break;
			case CLK_BTN_RESET : 
									clear_tamagotchi_window();
									display_map_rescale ( &session.display_map , TAMAGOTCHI_RESCALE);
									session_play( &session ); 
									break;
			default: break;
		}
	}
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
