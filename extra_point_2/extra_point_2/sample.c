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
static events_buffer_t events_buffer_staging_j;
static events_buffer_t events_buffer_staging_tp;
static events_buffer_t events_buffer_done;

static void run_done_area(void);

void do_tamagotchi_life_interrupt( void );
void do_user_events_interrupt_joystick( void );
void do_tamagotchi_custom_animation(void);
void do_user_events_interrupt_touch_panel(void);
void do_user_events_interrupt_adc(void);
void do_interrupt_adc(void);
void do_play_interrupt(void);

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
	LCD_Initialization();
	TP_Init();
	
	ADC_init();
	TouchPanel_Calibrate();
	session = session_init();
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);	
	
	events_buffer_staging_j = events_buffer_init();
	events_buffer_staging_tp = events_buffer_init();
	events_buffer_done = events_buffer_init();
}

void do_tamagotchi_life_interrupt(){
	tamagotchi_life_interrupt ( &session ) ;
}

void do_tamagotchi_custom_animation(){
	
	if( session.animation.animation_version == CUDDLES ){
		tamagotchi_cuddle_interrupt( &session );
	}
		
	if ( session.animation.animation_version == EATING ) {
		tamagotchi_eat_interrupt( &session ); 
	}
	
}

void do_interrupt_adc(void){
	volume_update(&session.sound);
}


void do_play_interrupt(void){
	play(&session.sound);
}
/*
Events queue
*/
void do_user_events_interrupt_joystick(){
	
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
	 
	//Serializzazione eventi utente
	while ( events_buffer_staging_j.size > 0 ) {
		event_p = pop(&events_buffer_staging_j);
		if( event_p->event == event_tmp.event ) {
			new_event = 0;
			event_p->trigger_number += 1; //Track the number of triggers that this event catches
			push( &events_buffer_staging_j , *event_p );
			break;
		} else {
			push( &events_buffer_done , *event_p ); //Go to DONE area ( serialization completed )
		}
	}
	
	if( event_tmp.event != VOID_EVENT && new_event ) {
		push( &events_buffer_staging_j , event_tmp );
	}
	
	run_done_area();
}

void do_user_events_interrupt_adc(){
	user_event_t event_tmp;
	event_tmp.trigger_number = 1;
	event_tmp.event = POTENTIOMETER_CHANGE;
	
	if( event_tmp.event != VOID_EVENT ) {
		push( &events_buffer_staging_tp , event_tmp );
	}
	
	run_done_area();
}
void do_user_events_interrupt_touch_panel(){
	
	int new_event = 1;
	display_map_t map_snapshoot = session.display_map;
	user_event_t event_tmp;
	user_event_t* event_p;
	event_tmp.trigger_number = 1;
	event_tmp.event = VOID_EVENT;
		
	//Touch over the tamagotchi, Note: the dimension is dynamic and it is stored in session.
	if(session.game_status_var == PLAY ){
		getDisplayPoint(&display, Read_Ads7846(), &matrix );
			if(display.y >= map_snapshoot.tamagotchi_y_start && display.y <= map_snapshoot.tamagotchi_y_start + map_snapshoot.dim_Y){
				if(display.x >= map_snapshoot.tamagotchi_x_start && display.x <= map_snapshoot.tamagotchi_x_start + map_snapshoot.dim_X){
					event_tmp.event = TOUCH_CUDDLES;
					display.x = 0;
					display.y = 0;
				}
			}
	}
	
	//Serializzazione eventi utente
	while ( events_buffer_staging_tp.size > 0 ) {
		event_p = pop(&events_buffer_staging_tp);
		if( event_p->event == event_tmp.event ) {
			new_event = 0;
			event_p->trigger_number += 1; //Track the number of triggers that this event catches
			push( &events_buffer_staging_tp , *event_p );
			break;
		} else {
			push( &events_buffer_done , *event_p ); //Go to DONE area ( serialization completed )
		}
	}
	
	if( event_tmp.event != VOID_EVENT && new_event ) {
		push( &events_buffer_staging_tp , event_tmp );
	}
	
	run_done_area();
}

/*
Events queue - run
*/
static void run_done_area(void){
	
	user_event_t* event_p;
	note_t note;
	
	while ( events_buffer_done.size > 0 ) {
		event_p = pop(&events_buffer_done);
		switch ( event_p->event ) {
			case SELECT_SNACK : 
									//Click sound
									note.durata = SEMIMINIMA;
									note.note = C5;
									sound_queue_put(&session.sound.sound_queue, note);
									play_wrapper(&session.sound);
									jc_selection( &session , SNACK_JC);  break;
			case SELECT_MEAL :
									//Click sound
									note.durata = SEMIMINIMA;
									note.note = C5;
									sound_queue_put(&session.sound.sound_queue, note);
									play_wrapper(&session.sound);
									jc_selection( &session , MEAL_JC); break;
			case CLK_BTN_SNACK :
									session.animation.animation_version = EATING;
									tamagotchi_user_wrapper_update(&session , event_p->event ); 
									break;
			case CLK_BTN_MEAL : 
									session.animation.animation_version = EATING;
									tamagotchi_user_wrapper_update(&session , event_p->event); 
									break;
			case CLK_BTN_RESET : 
									//Click sound
									note.durata = SEMIMINIMA;
									note.note = C4;
									sound_queue_put(&session.sound.sound_queue, note);
									play_wrapper(&session.sound);
			
									clear_tamagotchi_window();
									display_map_rescale ( &session.display_map , TAMAGOTCHI_RESCALE);
									session_play( &session ); 
									break;
			case TOUCH_CUDDLES:
									session.animation.animation_version = CUDDLES;
									tamagotchi_user_wrapper_cuddle( &session );
									break;
			case POTENTIOMETER_CHANGE: 
									/* ADC management */
									ADC_start_conversion();
									break;
			default: break;
		}
	}
	
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
