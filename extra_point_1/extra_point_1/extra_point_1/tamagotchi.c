#include "tamagotchi.h"

static age_t age_init( void ) ;
static status_t status_init( void );
static void age_2_lifetime( session_t* session );
static void add_1_second(session_t* session  );
static void session_reset ( session_t* session );
static void print_lifetime_LCD(session_t* session);
static void print_btn_play(void);
static void print_btn_reset(void);
static void print_battery( void );
static void print_battery_segments ( session_t * session ,battery_type battery);

static age_t age_init( void ) {
	age_t age;
	age.hours = age.minutes = age.seconds = 0;
	return age;
}

age_t age_init_values( int hours , int minutes , int seconds ) {
	age_t age;
	age.hours = hours;
	age.minutes = minutes;
	age.seconds = seconds;
	return age;
}

static status_t status_init( void ) {
	status_t status ;
	status.age = age_init();
	status.happiness = SESSION_HAPPINESS_INIT;
	status.satiety = SESSION_SATIETY_INIT;
	status.lifetime_timer_value = 0;
	return status;
}

session_t session_init( void ) {
	session_t session;
	LCD_Initialization();
	//sprintf(session.lifetimeStr , "NOT_VALID" );
	session.status = status_init();
	age_2_lifetime( &session );
	init_timer ( TAMAGOTCHI_TIMER_NUM , TAMAGOTCHI_TIMER_TC ) ;
	session.jc = VOID_JC;
	session.animation.animation_rescale = TAMAGOTCHI_RESCALE;
	session.animation.animation_version = HAPPY;
	session.display_map = display_map_default_init();
	GUI_Text (0,LIFETIME_SUB_TAG_Y, LIFETIME_STRING_SUB_TAG , Black , White);
	print_battery();
	session.in_motion = 0;
	session.game_status_var = INIT;
	return session;
}

static void session_reset ( session_t* session ) {
	disable_timer ( TAMAGOTCHI_TIMER_NUM ) ;
	reset_timer( TAMAGOTCHI_TIMER_NUM ) ;
	print_btn_reset();
	jc_selection( session , RESET_JC );

	//Simple run away animation
	session->animation.animation_rescale = TAMAGOTCHI_RESCALE;
	display_map_rescale( &session->display_map , TAMAGOTCHI_RESCALE );
	
	print_lcd_from_LRMATRIX (color_matrix_left[ SAD ], 
													 DISPLAY_CENTER_X + session->display_map.tamagotchi_x_start , 
													 session->display_map.tamagotchi_y_start , 
													 session->animation.animation_rescale );
	GUI_Text (DISPLAY_CENTER_X - 60 , DISPLAY_CENTER_Y , LIFETIME_STRING_GAME_OVER, Black , White );
	
	if( session->game_status_var != RESET) {
		return ;
	}
	
	session->status = status_init();
	age_2_lifetime( session );
	init_timer( TAMAGOTCHI_TIMER_NUM , TAMAGOTCHI_TIMER_TC );
  reset_timer(TAMAGOTCHI_TIMER_NUM);
	session->animation.animation_rescale = TAMAGOTCHI_RESCALE;
	session->animation.animation_version = HAPPY;
	session->game_status_var = INIT;
	joystick_enable();
}

void session_play( session_t* session ) {	
	if( session->game_status_var != INIT) {
		session->game_status_var = RESET;
		session_reset(session);
		return ;
	}
	//The first load of tamagotchi image is too expensive but the nexts are optimized with DELTA_MATRIX function
	print_lcd_from_LRMATRIX (color_matrix_left[session->animation.animation_version], 
													 session->display_map.tamagotchi_x_start , 
													 session->display_map.tamagotchi_y_start , 
													 session->animation.animation_rescale);
	print_lifetime_LCD(session);
	print_btn_play();
	print_battery_segments( session , SATIETY_BATTERY );
	print_battery_segments( session , HAPPINESS_BATTERY );
	
	//PLAY
	session->game_status_var = PLAY; 
	enable_timer ( TAMAGOTCHI_TIMER_NUM ) ;
}

/*
tamagotchi_life_interrupt
1. Update status
	- update lifetime
	- update levels of satitety and happiness 
2. Check if tamotchi is alive or is dead
*/
void tamagotchi_life_interrupt ( session_t* session ) {
	int number_of_animations = (int) ANIMATION_VOID;
	animation_map next_animation = ( ( (int) session->animation.animation_version + 1 ) % number_of_animations ) ;
	add_1_second( session ) ;
	session->status.lifetime_timer_value += 1;
	print_lifetime_LCD(session);
	
	if( session->status.lifetime_timer_value == TAMAGOTCHI_HAPPINESS_SATIETY_DECREASE_TIME ) {
		tamagotchi_happiness_update ( session , -1 );
		tamagotchi_satiety_update ( session , -1 );
		session->status.lifetime_timer_value = 0;
	}else if ( ! session->in_motion ){
		print_lcd_from_DELTAMATRIX (color_matrix_left[ session->animation.animation_version ],
															 color_matrix_left[ next_animation ], 
															 session->display_map.tamagotchi_x_start , 
															 session->display_map.tamagotchi_y_start , 
															 session->animation.animation_rescale);
		session->animation.animation_version = next_animation;
	}
	if ( session->status.happiness == 0 ||  session->status.satiety == 0){
		session->game_status_var = RESET;
		joystick_disable();
		session_reset(session);
	}
}

/*
	Handgling of JC events 
*/
void jc_selection(session_t* session , joystick_cursor jc){
	session->jc = jc;
	switch(jc) {
		case SNACK_JC: 	
			LCD_DrawBorder (DISPLAY_CENTER_X,USER_BUTTON_Y,DISPLAY_Y - 1,DISPLAY_Y - USER_BUTTON_Y , Red , 5);
			LCD_DrawBorder (0,USER_BUTTON_Y,DISPLAY_CENTER_X - 1,DISPLAY_Y - USER_BUTTON_Y , Black ,  5   );
			 break;
		case MEAL_JC: 
			LCD_DrawBorder (0,USER_BUTTON_Y,DISPLAY_CENTER_X - 1,DISPLAY_Y - USER_BUTTON_Y , Red ,  5   );
			LCD_DrawBorder (DISPLAY_CENTER_X,USER_BUTTON_Y,DISPLAY_Y - 1,DISPLAY_Y - USER_BUTTON_Y , Black , 5);
			 break;
		case RESET_JC: 	LCD_DrawBorder (0,USER_BUTTON_Y,DISPLAY_X - 1 ,DISPLAY_Y - USER_BUTTON_Y , Red , 5 );  break;
		default: break;
	}
}

/*
	Eat interraput
*/
void tamagotchi_eat_interrupt(session_t * session){
	int x_frame;
	if ( session->in_motion < IN_MOTION_NUM )
	{
		/*
		clear_tamagotchi_window();
		if( session->jc == SNACK_JC ){
			draw_snack();
		}
		else if( session->jc == MEAL_JC ){
			draw_meal();
		}
		*/
		
		x_frame = session->in_motion * 15;
		if( session->in_motion == IN_MOTION_NUM -1 ){
			x_frame = ( session->in_motion -1 ) * 15;
		}
		clear_window(0, TAMAGOTCHI_SQUARE_Y1 + (TAMAGOTCHI_SQUARE_Y_LEN - IN_MOTION_Y ) / 2 , IN_MOTION_Y , x_frame);
		print_lcd_from_FRAME( 0 +  x_frame , 
											TAMAGOTCHI_SQUARE_Y1 + (TAMAGOTCHI_SQUARE_Y_LEN - IN_MOTION_Y ) / 2 , 
											 ( session->in_motion % 2 ) + 1);
		session->in_motion += 1 ;
		//clear_window ( 0 + ( session.in_motion - 1 ) * 15 , TAMAGOTCHI_SQUARE_Y1 + (TAMAGOTCHI_SQUARE_Y_LEN - IN_MOTION_Y ) / 2 , IN_MOTION_Y , 15 ); 
		return;
	}
	else {
		clear_tamagotchi_window();
		session->in_motion = 0; //Unlock change of animation 
		if ( session->jc == MEAL_JC ){
			tamagotchi_satiety_update ( session , 1 );
		}
		else if ( session->jc == SNACK_JC ){
			tamagotchi_happiness_update ( session , 1 );
			tamagotchi_satiety_update ( session , 0); //Only for refresh animation
		}
		disable_timer(TAMAGOTCHI_TIMER_CUSTOM_ANIMATION_NUM);
		joystick_enable();
	}
}

/*
Update lifetime string
*/
static void age_2_lifetime( session_t* session ){
	sprintf( (char * ) session->lifetimeStr , LIFETIME_STRING_FORMAT , session->status.age.hours,
																			 session->status.age.minutes,
																			 session->status.age.seconds);
}

void tamagotchi_happiness_update ( session_t* session , int update ){
	if ( update == 0 ) {
		return;
	}
	session->status.happiness += update;
	print_battery_segments( session , HAPPINESS_BATTERY );
}

void tamagotchi_user_wrapper_update( session_t* session , event_type event ){
	
	//check out of bound
	if ( (session->status.happiness + 1 > SESSION_HAPPINESS_INIT && session->jc == SNACK_JC )
		|| ( session->status.satiety + 1 > SESSION_SATIETY_INIT && session->jc == MEAL_JC )  ) 
		return;
	
	joystick_disable();
	init_timer (TAMAGOTCHI_TIMER_CUSTOM_ANIMATION_NUM , TAMAGOTCHI_TIMER_CUSTOM_ANIMATION_TC );
	enable_timer(TAMAGOTCHI_TIMER_CUSTOM_ANIMATION_NUM);
	clear_tamagotchi_window();
	
	if( session->jc == SNACK_JC ){
		draw_snack();
	}
	else if( session->jc == MEAL_JC ){
		draw_meal();
	}
	
	print_lcd_from_FRAME(0, 
											TAMAGOTCHI_SQUARE_Y1 + (TAMAGOTCHI_SQUARE_Y_LEN - IN_MOTION_Y ) / 2 , 
											( session->in_motion % 2 ) + 1);
	session->in_motion += 1 ;
}

void tamagotchi_satiety_update ( session_t* session , int update ){
	int rescale_tmp, number_of_animations = (int)ANIMATION_VOID;
	animation_map next_animation = ( ( (int) session->animation.animation_version + 1 ) % number_of_animations ) ;
	//if ( update == 0 ) { return; } The update could be used also for only modifing animation
	session->status.satiety += update;
	if( update != 0 ) print_battery_segments( session, SATIETY_BATTERY );	
	rescale_tmp = (int ) round ( (float)TAMAGOTCHI_RESCALE * ((float) session->status.satiety / SESSION_SATIETY_INIT ));
	
	//No rescale update if in motion
	if ( session->in_motion != 0 ) {
		return;
	}
	
	if(rescale_tmp > 0 
		&& rescale_tmp != session->animation.animation_rescale ) {
		session->animation.animation_rescale = rescale_tmp;
		display_map_rescale ( &session->display_map , session->animation.animation_rescale);
		//No rescale update if in motion
		if ( session->in_motion != 0 ) {
			return;
		}
		print_lcd_from_LRMATRIX (color_matrix_left[ next_animation ], 
													 session->display_map.tamagotchi_x_start , 
													 session->display_map.tamagotchi_y_start , 
													 session->animation.animation_rescale);
	}
	else {
		//No rescale update if in motion
		if ( session->in_motion != 0 ) {
			return;
		} 
		print_lcd_from_DELTAMATRIX (color_matrix_left[ session->animation.animation_version ],
															 color_matrix_left[ next_animation ], 
															 session->display_map.tamagotchi_x_start , 
															 session->display_map.tamagotchi_y_start , 
															 session->animation.animation_rescale);
	}
	session->animation.animation_version = next_animation;
}
	
static void print_lifetime_LCD(session_t* session){
	GUI_Text (session->display_map.lifetime_x_start,
						session->display_map.lifetime_y_start,
						(unsigned char*) session->lifetimeStr,
						session->status.lifetime_timer_value%2==0 ? White : Black, 
						session->status.lifetime_timer_value%2==0 ? Blue : White );
}

static void print_btn_play(void){
	LCD_DrawRectangle (0,USER_BUTTON_Y,DISPLAY_CENTER_X - 1,DISPLAY_Y - USER_BUTTON_Y , Grey   );

	LCD_DrawRectangle (DISPLAY_CENTER_X,USER_BUTTON_Y,DISPLAY_Y - 1,DISPLAY_Y - USER_BUTTON_Y , Grey);
	GUI_Text (0,USER_BUTTON_Y + 2*DISPLAY_SECTION_SPACE , LIFETIME_STRING_BNT_TAG , Black , Grey);
	LCD_DrawBorder (0,USER_BUTTON_Y,DISPLAY_CENTER_X - 1,DISPLAY_Y - USER_BUTTON_Y , Black ,  4   );
	LCD_DrawBorder (DISPLAY_CENTER_X,USER_BUTTON_Y,DISPLAY_Y - 1,DISPLAY_Y - USER_BUTTON_Y , Black ,4);
}

static void print_btn_reset(void){
	LCD_DrawRectangle (0,USER_BUTTON_Y,DISPLAY_X - 1 ,DISPLAY_Y - USER_BUTTON_Y , Grey );
	GUI_Text (0,USER_BUTTON_Y + 2*DISPLAY_SECTION_SPACE , LIFETIME_STRING_BNT_RES , Black , Grey);
	LCD_DrawBorder (0,USER_BUTTON_Y,DISPLAY_X - 1 ,DISPLAY_Y - USER_BUTTON_Y , Black , 4 );

}

static void print_battery( void ){
	int i = 0;
	LCD_DrawRectangle ( 20 , BATTERY_Y_START , BATTERY_L , BATTERY_H , Grey);
	LCD_DrawBorder ( 20 ,BATTERY_Y_START, BATTERY_L , BATTERY_H , Black , BATTERY_BORDER_RADIUS);
	while ( i < BATTERY_H / 2 ){
		LCD_SetPoint(20 + BATTERY_L , BATTERY_Y_START + (BATTERY_H / 4) + i , Black  );
		LCD_SetPoint(20 + BATTERY_L + 1 , BATTERY_Y_START + (BATTERY_H / 4) + i , Black  );
		LCD_SetPoint(20 + BATTERY_L + 2 , BATTERY_Y_START + (BATTERY_H / 4) + i , Black  );
		LCD_SetPoint(20 + BATTERY_L + 3 , BATTERY_Y_START + (BATTERY_H / 4) + (i++) , Black  );
	}
	i = 0;
	LCD_DrawRectangle ( BATTERY_L + 60  ,BATTERY_Y_START , BATTERY_L , BATTERY_H , Grey);
	LCD_DrawBorder ( BATTERY_L + 60 ,BATTERY_Y_START , BATTERY_L , BATTERY_H , Black , BATTERY_BORDER_RADIUS);
	
	while ( i < BATTERY_H / 2 ){
		LCD_SetPoint(60 + BATTERY_L*2 , BATTERY_Y_START + (BATTERY_H / 4) + i , Black  );
		LCD_SetPoint(60 + BATTERY_L*2 + 1 , BATTERY_Y_START + (BATTERY_H / 4) + i , Black  );
		LCD_SetPoint(60 + BATTERY_L*2 + 2 , BATTERY_Y_START + (BATTERY_H / 4) + i , Black  );
		LCD_SetPoint(60 + BATTERY_L*2 + 3 , BATTERY_Y_START + (BATTERY_H / 4) + (i++) , Black  );
	}
	
}

static void print_battery_segments ( session_t * session ,battery_type battery){
	int i , x_start_happiness , x_start_satiety, y_start ; 
	
	y_start = BATTERY_Y_START + BATTERY_BORDER_RADIUS;
	x_start_happiness = 20 + BATTERY_BORDER_RADIUS ; 
	x_start_satiety = BATTERY_L + 60 + BATTERY_BORDER_RADIUS ; 
	
	i = 0;
	
	switch ( battery ) {
		case SATIETY_BATTERY : 
			//Satiety
			while ( i < SESSION_SATIETY_INIT ) {
				if ( i < session->status.satiety ) {
					LCD_DrawRectangle ( x_start_satiety  ,y_start , SEGMENT_L , SEGMENT_H , Black);
					LCD_DrawBorder ( x_start_satiety ,y_start , SEGMENT_L , SEGMENT_H , Grey , SEGMENT_BORDER_RADIUS);
				}else {
					LCD_DrawRectangle ( x_start_satiety  ,y_start , SEGMENT_L , SEGMENT_H , Grey);
				}
				x_start_satiety += SEGMENT_L;
				i += 1 ;
			}
			break;
		case HAPPINESS_BATTERY: 
			//Happiness
			while ( i < SESSION_HAPPINESS_INIT ) {
				if( i < session->status.happiness ) {
					LCD_DrawRectangle ( x_start_happiness  ,y_start , SEGMENT_L , SEGMENT_H , Black);
					LCD_DrawBorder ( x_start_happiness ,y_start , SEGMENT_L , SEGMENT_H , Grey , SEGMENT_BORDER_RADIUS);
				} else {
					LCD_DrawRectangle ( x_start_happiness  ,y_start , SEGMENT_L , SEGMENT_H , Grey);
					//LCD_DrawBorder ( x_start_happiness ,y_start , SEGMENT_L , SEGMENT_H , Grey , SEGMENT_BORDER_RADIUS);
				}
				x_start_happiness += SEGMENT_L;
				i += 1 ;
			}
			break;
	}
}

/*
Update age ( add 1 second to age)
*/
static void add_1_second(session_t* session  ){
	if( session->status.age.seconds == 59 ) {
		session->status.age.seconds = 0;
		
		//Add 1 minute
		if( session->status.age.minutes == 59 ){
			session->status.age.minutes = 0;
			session->status.age.hours += 1;
		}else{
			session->status.age.minutes += 1;
		}
		
	}else{
		session->status.age.seconds += 1;
	}
	age_2_lifetime ( session ) ;
}
