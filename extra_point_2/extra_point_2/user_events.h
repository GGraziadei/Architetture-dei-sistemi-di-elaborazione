#ifndef _user_events_H
#define _user_events_H

#include<stdio.h>
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "TouchPanel/TouchPanel.h"

#define BUFFER_SIZE 10

//Polling sul joystick
#define user_event_tS_TIMER_NUM "RIT"
#define user_event_tS_TIMER_TC  0x1312D0 /* 50ms * 25MHz = 0x1312D0 */

#define USER_EVENT_TP_TIMER_NUM 2
#define USER_EVENT_TP_TIMER_TC 12500000 * 0.01 //10ms * 12.5 Mhz = 25 * 8 

#define USER_EVENT_PRIORITY 1

typedef enum {
	SELECT_SNACK,
	SELECT_MEAL,
	CLK_BTN_SNACK, 
	CLK_BTN_MEAL,
	CLK_BTN_RESET,
	TOUCH_CUDDLES,
	POTENTIOMETER_CHANGE,
	VOID_EVENT
} event_type;

typedef enum {
	SNACK_JC,
	MEAL_JC,
	RESET_JC,
	VOID_JC
} joystick_cursor;

typedef struct {
	int trigger_number; 
	event_type event;
} user_event_t;

//LIFO queue
typedef struct{
	int index , size;
	user_event_t buffer[BUFFER_SIZE];
} events_buffer_t;

extern void user_events_init(void);
extern events_buffer_t events_buffer_init(void);
extern events_buffer_t* push(events_buffer_t* buffer, user_event_t event);
extern user_event_t* pop(events_buffer_t* buffer);

extern void user_events_disable(void);
extern void user_events_enable(void);

#endif
