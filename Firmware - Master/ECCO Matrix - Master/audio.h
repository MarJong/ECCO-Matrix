#ifndef AUDIO_H_
#define AUDIO_H_

#define SONG_NUM		10
#define NOTEN_NUM		46

#define AUDIO_ON		TCCR1B |= (1 << CS10);
#define AUDIO_OFF		TCCR1B &= ~(1 << CS10);

#define SPEAKER_L_OFF		TIMSK1 &= ~(1 << OCIE1A)
#define SPEAKER_L_ON		TIMSK1 |= (1 << OCIE1A)

#define SPEAKER_R_OFF		TIMSK1 &= ~(1 << OCIE1B)
#define SPEAKER_R_ON		TIMSK1 |= (1 << OCIE1B)

#define SPEAKER_OFF			TIMSK1 &= ~((1 << OCIE1A) | (1 << OCIE1B))
#define SPEAKER_ON			TIMSK1 |= ((1 << OCIE1A) | (1 << OCIE1B))

#define FLAG_ISPLAYING		0x01
#define FLAG_REPEAT			0x02
#define FLAG_BEAT_10		0x04
#define FLAG_BEAT_20		0x08
#define FLAG_RANDOM			0x10
#define FLAG_AUDIO_MENU		0x80

typedef struct {
	uint8_t note, len;
} note_t;

typedef struct {
	char name[21];
	uint16_t len;
	note_t *noten;
} lied_t;
	
extern volatile uint16_t audio_ocr_val_l, audio_ocr_val_r;


// init
void audio_init(void);

// stop
void audio_stop(void);

// lied abspielen
void audio_play(uint8_t l, uint8_t rep, uint8_t binbeat);

// zufallslieder abspielen bis audio_stop
void audio_play_random(void);

// alle 1/64s aufrufen
void audio_step(void);

// menü interface
void audio_menuStart(void);
void audio_menuInput(void);
void audio_menuDraw(void);

#endif /* AUDIO_H_ */