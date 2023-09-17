#ifndef ANIMATION_H_
#define ANIMATION_H_

typedef struct {
	void (*func_init)(void);		// init function
	void (*func_step)(void);		// animation step function
	uint8_t step_delay;				// delay zwischen fun_step(); 0 ... volle framerate, alle n + 1 frames
	char name[21];					// bezeichnung
} anim_list_t;

#define ANIM_PLAY_LEN		60	// alle x sek neue animation in idle

#define ANIM_FLAG_RUNNING			0x01
#define ANIM_FLAG_RANDOM			0x02

// init
void anim_init(void);

// zufalls animationen ein/aus
void anim_setRandom(uint8_t r);

// jeden frame aufrufen
void anim_frame(void);

// zufällige neue animation
void anim_random(uint32_t sec);

// alle farben auf 0
void anim_clear_all(void);

// animation num abspielen
void anim_play(uint8_t num);

// menü
void anim_menuStart(void);
void anim_menuInput(void);



// regen
void anim_regen_step(void);

// regen 2
void anim_regen2_init(void);
void anim_regen2_step(void);

// ecco startup
void anim_ecco_init(void);
void anim_ecco_step(void);

// fader 1
void anim_fader1_init(void);
void anim_fader1_step(void);

//
void anim_disco_step(void);

//
void anim_armding_init(void);
void anim_armding_step(void);

//
void anim_linie_init(void);
void anim_linie_step(void);

//
void anim_mpixel_init(void);
void anim_mpixel_step(void);

//
void anim_jukebox_start(void);
void anim_jukebox_step(void);

//
void anim_pfeile_init(void);
void anim_pfeile_step(void);

//
void anim_regenbogen_init();
void anim_regenbogen_step();

#endif /* ANIMATION_H_ */