#ifndef _TAMAGOTCHI_H
#define _TAMAGOTCHI_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "timer/timer.h"
#include "GLCD/GLCD.h" 
#include "user_events.h"
#include "tamagotchi_animations.h"
#include "tamagotchi_sound.h"

#define LIFETIME_STRING_LEN CHARS_PER_ROW+1
#define LIFETIME_STRING_FORMAT  	"        Age: %02d:%02d:%02d         "
#define LIFETIME_STRING_SUB_TAG 	"   HAPPINESS       SATIETY    "
#define LIFETIME_STRING_BNT_TAG 	"      MEAL          SNACK     "
#define LIFETIME_STRING_BNT_RES 	"            RESET             "
#define LIFETIME_STRING_GAME_OVER "GAME OVER !"

#define TAMAGOTCHI_HAPPINESS_SATIETY_DECREASE_TIME 5
#define SESSION_HAPPINESS_INIT 5
#define SESSION_SATIETY_INIT 5

#define TAMAGOTCHI_TIMER_NUM 1
#define TAMAGOTCHI_TIMER_TC  0xBEBC20 // 1 * 12500000 ; 12.5 MHz

#define TAMAGOTCHI_TIMER_CUSTOM_ANIMATION_NUM 0 
#define TAMAGOTCHI_TIMER_CUSTOM_ANIMATION_TC TAMAGOTCHI_TIMER_TC/IN_MOTION_NUM  // 1 * 12500000 ; 12.5 MHz / 4 interruzioni per animazione
#define TAMAGOTCHI_TIMER_CUSTOM_CRUDDLE_TC ( 2 * 12500000 ) / CUDDLES_FRAME_N  // 2 * 12500000 ; 12.5 MHz / 6 interruzioni per animazione


typedef enum {
	INIT,
	PLAY,
	RESET
} game_status;

typedef enum {
	SATIETY_BATTERY,
	HAPPINESS_BATTERY
} battery_type;

typedef struct age {
	int hours;
	int minutes;
	int seconds;
} age_t;

typedef struct status {
	age_t age;
	int happiness;
	int satiety;
	int lifetime_timer_value;
} status_t;

typedef struct session {
	status_t status;
	unsigned char lifetimeStr[LIFETIME_STRING_LEN];
	joystick_cursor jc;
	game_status game_status_var;
	animation_t animation;
	display_map_t display_map;
	sound_t sound;
	int in_motion;
} session_t;

extern age_t age_init_values( int hours , int minutes , int seconds ) ;
extern session_t session_init( void );
extern void tamagotchi_satiety_update ( session_t* session , int update );
extern void tamagotchi_happiness_update ( session_t* session , int update );
extern void tamagotchi_life_interrupt ( session_t* session ) ;
extern void session_play( session_t* session );
extern void jc_selection(session_t* session , joystick_cursor jc);
extern void tamagotchi_user_wrapper_update( session_t* session , event_type event );
extern void tamagotchi_eat_interrupt(session_t * session);
extern void tamagotchi_cuddle_interrupt(session_t * session);
extern void tamagotchi_user_wrapper_cuddle( session_t* session);

//static game_status GAME_STATUS_VAR = RESET; // FSM: RESET->INIT->PLAY->RESET

#endif
