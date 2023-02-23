#include "tamagotchi_sound.h"
#include <math.h>

static sound_queue_t sound_queue_init(void);
static void set_note(sound_t* sound, note_t note);

const uint16_t SinTable[SIN_POINT_N] =                                       
{
	512,	583,	653,	720,	783,	841,	
	892,	936,	972,	999,	1016,	1024,	
	1021,	1009,	987,	955,	915,	868,	
	813,	752,	687,	618,	548,	476,	
	406,	337,	272,	211,	156,	109,	
	69,	37,	15,	3,	0,	8,	25,	52,	88,	
	132,	183,	241,	304,	371,	441
};

const int freqs[FREQ_N]={2120,1890,1684,1592,1417,1263,1125,1062};
/*
262Hz	k=2120		c4
294Hz	k=1890		
330Hz	k=1684		
349Hz	k=1592		
392Hz	k=1417		
440Hz	k=1263		
494Hz	k=1125		
523Hz	k=1062		c5

*/

static sound_queue_t sound_queue_init(void){
	sound_queue_t queue;
	queue.read = BUFFER_N - 1;
	queue.write = 0;
	queue.size = 0;
	return queue;
}

note_t sound_queue_get(sound_queue_t* queue){
	note_t res;
	res.note = C4;
	res.durata = 0;
	
	if(queue->size == 0 ) {
		return res;
	}
	queue->size -= 1;
	queue->read = (queue->read + 1) % BUFFER_N;
	return queue->buffer[queue->read];
}

void sound_queue_put(sound_queue_t* queue, note_t note){
	
	if(queue->size == BUFFER_N-1 ) {
		return;
	}
	queue->size += 1;
	queue->buffer[queue->write] = note;
	queue->write = (queue->write + 1) % BUFFER_N; 
}

sound_t sound_init(void){
	sound_t s;
	s.AD_last = 0xFF;
	s.volume_value = VOLUME_MAX;
	s.current_note = C4;
	s.frequency = freqs[s.current_note];
	s.sound_queue = sound_queue_init();
	draw_speaker_on(SPEAKER_X_START, SPEAKER_Y_START);
	draw_speaker_level(SPEAKER_X_START , SPEAKER_Y_START , s.volume_value);
	return s;
}

/* k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45) */

void volume_update(sound_t* sound){
	unsigned short AD_current; 
	uint8_t last_volume_value = sound->volume_value;
	
	AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
	
  if(AD_current == sound->AD_last){
		uint8_t volume_tmp = (int) round( (double) ( AD_current * VOLUME_MAX ) /0xFFF );
		sound->AD_last = AD_current;	
		if( volume_tmp != sound->volume_value ) {
			sound->volume_value = volume_tmp;
			if(last_volume_value == 0 ){
				clear_window(SPEAKER_X_START, SPEAKER_Y_START, SPEAKER_L, SPEAKER_H);
				draw_speaker_on(SPEAKER_X_START, SPEAKER_Y_START);
			}
			draw_speaker_level(SPEAKER_X_START , SPEAKER_Y_START , sound->volume_value);
			if(sound->volume_value == 0){
				draw_speaker_off(SPEAKER_X_START, SPEAKER_Y_START);
			}
		}
  }else{
		sound->AD_last = AD_current;
	}
}

static void set_note(sound_t* sound, note_t note){
	sound->current_note = note.note;
	sound->frequency = freqs[sound->current_note];
	sound->durata = SEMIMINIMA;
	sound->durata = note.durata;
	init_timer(SOUND_TIM_N, sound->frequency);
}

void play_wrapper(sound_t* sound){
	
	if(sound->sound_queue.size <= 0 ){
			return;
	}
	
	note_t note = sound_queue_get(&sound->sound_queue);
	//note_enum note = n.note;
	
	set_note(sound, note);
	reset_timer(SOUND_TIM_N);
	enable_timer(SOUND_TIM_N);
}

void play(sound_t* sound){
	static int t = 0;
	static int index = 0;

	//  SinTable[index]:SIN_MAX = x:NEW_MAX
	float sin_value =  (float) sound->volume_value * LSB;
	sin_value *= SinTable[t++];
	sin_value /= SIN_MAX;
	
	uint16_t value = (uint16_t) round(sin_value * VOLUME_SAT );
	
	LPC_DAC->DACR = (value<<6); 
	if(t==SIN_POINT_N)t=0;
	
	index++;
	
	if(index == ( ( SOUND_TIM_F * SEMIMINIMA ) /  sound->frequency ) ){ //Each note 0.3333s
		disable_timer(SOUND_TIM_N);
		reset_timer(SOUND_TIM_N);
		index = 0;
		play_wrapper(sound);
	}
	
}

