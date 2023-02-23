#include "LPC17xx.h"
#include "ADC/adc.h"
#include "timer/timer.h"
#include "tamagotchi_animations.h"

#define SOUND_TIM_N 3
#define SOUND_TIM_F 25000000
#define SIN_POINT_N 45
#define FREQ_N 8

#define VOLUME_MAX 8
#define DAC_MAX 1024
#define SIN_MAX DAC_MAX
#define LSB DAC_MAX/VOLUME_MAX
#define VOLUME_SAT 77/80

#define SEMIMINIMA 1/4
#define CROMA SEMIMINIMA/2

#define BUFFER_N 20


typedef enum {
	C4,
	D,
	E,
	F,
	G,
	A,
	B,
	C5,
	NOTE_VOID
} note_enum;

typedef struct {
	note_enum note;
	uint8_t durata;
}note_t;

typedef struct {
	int read,write,size;
	note_t buffer[BUFFER_N];
} sound_queue_t;
	

typedef struct {
	uint8_t volume_value;
	note_enum current_note;
	sound_queue_t sound_queue;
	float durata;
	int frequency; //K che si ottiene per una rispettiva frequenza con un timer impostato a 25MHz
	unsigned short AD_last;
} sound_t;

extern sound_t sound_init(void);
extern void play_wrapper(sound_t* sound);
extern note_t sound_queue_get(sound_queue_t* queue);
extern void sound_queue_put(sound_queue_t* queue, note_t note);
extern void volume_update(sound_t* sound);
extern void play(sound_t* sound);
