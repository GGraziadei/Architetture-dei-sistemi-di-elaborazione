#include "user_events.h"

void user_events_init(){
	TP_Init();
	//TouchPanel_Calibrate();
	init_RIT(user_event_tS_TIMER_TC);				/* RIT Initialization 50 msec       	*/			    
	joystick_enable();
}

events_buffer_t events_buffer_init(void){
	events_buffer_t buffer;
	buffer.index = 0;
	buffer.size = 0;
	return buffer;
}

//PUSH
// buffer_index = next index
events_buffer_t* push(events_buffer_t* buffer, user_event_t event){
	buffer->buffer [ buffer->index ] = event;
	buffer->index = ( buffer->index + 1 ) % BUFFER_SIZE;
	buffer->size += 1 ;
	return buffer;
}

//POP
user_event_t* pop(events_buffer_t* buffer){
	if ( buffer->size == 0 ) {
		return (user_event_t*) NULL;
	}
	buffer->size -= 1;
	if ( buffer->index == 0 ) {
			buffer->index = BUFFER_SIZE;
	}
	return &buffer->buffer[-- buffer->index];
}

void joystick_enable(){
	/* joystick select functionality ( only left , right, select) */
	
	//Select
  LPC_PINCON->PINSEL3 &= ~(1<<25);	
	LPC_GPIO1->FIODIR   &= ~(1<<25);
	
	//Right
  LPC_PINCON->PINSEL3 &= ~(1<<28);	
 	LPC_GPIO1->FIODIR   &= ~(1<<28);
	
	//Left
  LPC_PINCON->PINSEL3 &= ~(1<<27);	
	LPC_GPIO1->FIODIR   &= ~(1<<27);
	
	enable_RIT();													/* RIT enabled												*/		
}

void joystick_disable(){

	//Select
  LPC_PINCON->PINSEL3 |= (1<<25);	
	LPC_GPIO1->FIODIR   |= (1<<25);
	
	//Right
  LPC_PINCON->PINSEL3 |= (1<<28);	
	LPC_GPIO1->FIODIR   |= (1<<28);
	
	//Left
  LPC_PINCON->PINSEL3 |= (1<<27);	
	LPC_GPIO1->FIODIR   |= (1<<27);
	
	disable_RIT();												/* RIT disabled												*/
	reset_RIT();
}
