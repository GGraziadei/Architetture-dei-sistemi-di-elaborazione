#ifndef _TAMAGOTCHI_ANIMATIONS_H
#define _TAMAGOTCHI_ANIMATIONS_H

#include "GLCD/GLCD.h"

#define DISPLAY_X MAX_X 
#define DISPLAY_Y MAX_Y
#define DISPLAY_CENTER_X (DISPLAY_X/2) 
#define DISPLAY_CENTER_Y (DISPLAY_Y/2) 
#define DISPLAY_SECTION_SPACE 12

#define TAMAGOTCHI_REAL_DIM_X 28
#define TAMAGOTCHI_REAL_DIM_Y 24

#define TAMAGOTCHI_RESCALE 6
#define TAMAGOTCHI_DIM_X TAMAGOTCHI_RESCALE * TAMAGOTCHI_REAL_DIM_X
#define TAMAGOTCHI_DIM_Y TAMAGOTCHI_RESCALE * TAMAGOTCHI_REAL_DIM_Y

#define TAMAGOTCHI_SQUARE_Y1 75
#define TAMAGOTCHI_SQUARE_Y2 245
#define TAMAGOTCHI_SQUARE_Y_LEN TAMAGOTCHI_SQUARE_Y2-TAMAGOTCHI_SQUARE_Y1
#define TAMAGOTCHI_SQUARE_X_LEN DISPLAY_X

#define TAMAGOTCHI_DELTA_SQUARE_Y (TAMAGOTCHI_SQUARE_Y_LEN-TAMAGOTCHI_DIM_Y)/2
#define TAMAGOTCHI_DELTA_SQUARE_X (TAMAGOTCHI_SQUARE_X_LEN-TAMAGOTCHI_DIM_X)/2

#define CHARS_PER_ROW 30

#define LIFETIME_SUB_TAG_Y (TAMAGOTCHI_SQUARE_Y1 - DISPLAY_SECTION_SPACE)
#define USER_BUTTON_Y (TAMAGOTCHI_SQUARE_Y2 + DISPLAY_SECTION_SPACE)

#define BATTERY_L 80
#define SEGMENT_L ( BATTERY_L - 2*BATTERY_BORDER_RADIUS ) / 5 
#define BATTERY_Y_START TAMAGOTCHI_SQUARE_Y1 - BATTERY_H - 15 
#define BATTERY_H 40
#define BATTERY_BORDER_RADIUS 5
#define SEGMENT_H BATTERY_H - 2 * BATTERY_BORDER_RADIUS
#define SEGMENT_BORDER_RADIUS 2

#define IN_MOTION_X 180
#define IN_MOTION_Y 130
#define IN_MOTION_NUM 4

#define SNACK_X 20
#define SNACK_Y 17

#define MEAL_X 20
#define MEAL_Y 20

#define CUDDLES_X 180
#define CUDDLES_Y 147
#define CUDDLES_FRAME_N 6

#define SPEAKER_X_START DISPLAY_CENTER_X - 4
#define SPEAKER_Y_START BATTERY_Y_START + ( BATTERY_H/2 ) - 6
#define SPEAKER_L 13
#define SPEAKER_H 12


typedef enum {
	HAPPY,
	EARS_DOWN,
	SAD,
	ANIMATION_VOID,
	EATING,
	CUDDLES
} animation_map;

typedef struct {
	animation_map animation_version;
	int animation_rescale;
} animation_t;

typedef struct {
	int dim_X, dim_Y;
	int tamagotchi_square_y1,tamagotchi_square_y2;
	int tamagotchi_x_start,tamagotchi_y_start;
	int  lifetime_x_start,lifetime_y_start;
} display_map_t;

extern void print_lcd_from_LRMATRIX (uint16_t matrix[][TAMAGOTCHI_REAL_DIM_X/2], int x_start, int y_start , int tamagotchi_rescale);
extern void print_lcd_from_DELTAMATRIX ( uint16_t matrix_start[][TAMAGOTCHI_REAL_DIM_X/2] , uint16_t matrix_end[][TAMAGOTCHI_REAL_DIM_X/2], int x_start, int y_start , int tamagotchi_rescale );
extern display_map_t display_map_default_init(void);
extern void display_map_rescale( display_map_t* map, int new_rescale);
extern void clear_tamagotchi_window(void) ;
extern void draw_meal( void );
extern void draw_snack( void );
extern void print_lcd_from_FRAME(int x_start , int y_start , int frame_number);
extern void clear_window(int x_start , int y_start , int h , int w );
extern void draw_cuddles( int start_frame, int end_frame );

extern void draw_speaker_off(int x_start, int y_start);
extern void draw_speaker_on(int x_start, int y_start);
extern void draw_speaker_level(int x_start, int y_start, int volume_value);

extern uint16_t color_matrix_left[ANIMATION_VOID][TAMAGOTCHI_REAL_DIM_Y][TAMAGOTCHI_REAL_DIM_X/2];

#endif
