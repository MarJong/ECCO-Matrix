#include <avr/io.h>
#include <stdlib.h>

#include "master.h"
#include "audio.h"
#include "audio_lieder.h"
#include "lcd.h"
#include "ui_lcd.h"
#include "ps2controller.h"
#include "animation.h"

volatile uint16_t audio_ocr_val_l, audio_ocr_val_r;
uint8_t audio_flags;

#define SLD(x, y)			{x, sizeof(y) / 2, (note_t*)&y}

lied_t song_list[SONG_NUM] =	{
									SLD("River flows in you", song_river_flows_in_you),
									SLD("Ungarischer Tanz 5", song_hungarian_dance_5),
									SLD("Nyan Cat", lied_nyan_cat),
									SLD("Contra Stage 1", song_contra_1),
									SLD("Cara Mia", song_turret_opera),
									SLD("Chrono Trigger", song_chrono_trigger_theme),
									SLD("Morrowind", song_morrowind),
									SLD("Air Suite Nr.3", song_airsuite3),
									SLD("Halle des Bergk\x94nigs", song_inderhalle),
									SLD("Yaketi Sax", song_yaketi)
								};

// unterstütze noten
// enum noten_namen {	h3 = 0, ais3, a3, gis3, g3, fis3, f3, e3, dis3, d3, cis3, c3,
// 					h2, ais2, a2, gis2, g2, fis2, f2, e2, dis2, d2, cis2, c2,					
// 					h1, ais1, a1, gis1, g1, fis1, f1, e1, dis1, d1, cis1, c1,						
// 					h0, ais0, a0, gis0, g0, fis0, f0, e0, dis0,								
// 					pause,														
// 					NOTEN_NUM										
// 					};

// timer compare werte für noten
uint16_t noten_ocr_val[NOTEN_NUM] =	{	5062, 5363, 5682, 6020, 6378, 6757, 7159, 7584, 8035, 8513, 9019, 9556,
										10124, 10726, 11364, 12039, 12755, 13514, 14317, 15169, 16071, 17026, 18039, 19111,
										20248, 21452, 22727, 24079, 25511, 27027, 28635, 30337, 32141, 34052, 36077, 38223,
										40495, 42903, 45455, 48157, 51021, 54055, 57269, 60674, 64283, 0
										};
										
uint16_t noten_ocr_binaural_mod[NOTEN_NUM] =	{
10, 11, 13, 14, 16, 18, 20, 23, 26, 29, 33, 36, 41, 46,
52,59, 66, 73, 83, 92, 104, 117, 131, 148, 165, 185, 209, 234,
263, 296, 331, 373, 419, 471, 528,
593, 667, 749, 841, 946, 1063, 1195, 1343, 1510, 0, 0
											};

uint16_t audio_pos = 0;			// position im lied
uint8_t audio_song = 0;			// aktuelles lied

// alle 16ms aufrufen
void audio_step(void)
{
	static uint8_t nlen = 0;
	uint8_t npos = 0;
	
	if(!isset(audio_flags, FLAG_ISPLAYING))
		return;
	
	if(nlen)
	{
		nlen--;
		if(!nlen)
		{
			SPEAKER_OFF;
		}
		return;
	}
		
	if(audio_pos >= song_list[audio_song].len)
	{
			
		if(isset(audio_flags, FLAG_REPEAT))
		{
			audio_pos = 0;
			
			if(isset(audio_flags, FLAG_RANDOM))
				audio_song = rand() % SONG_NUM;
		}			
		else
		{
			cbi(audio_flags, FLAG_ISPLAYING);
			if(isset(audio_flags, FLAG_AUDIO_MENU))
			{
				audio_menuDraw();
				audio_stop();
				sbi(audio_flags, FLAG_AUDIO_MENU);
			}
			else
				audio_stop();				
		}			
		return;
	}		
	
	npos = pgm_read_byte(&song_list[audio_song].noten[audio_pos].note);
	audio_ocr_val_r = audio_ocr_val_l = noten_ocr_val[npos];
	nlen = pgm_read_byte(&song_list[audio_song].noten[audio_pos].len);
		
	
	if(isset(audio_flags, FLAG_AUDIO_MENU))
	{
		char temp[22];
		div_t res;
		uint8_t i = 0;
		
		res = div(audio_pos * 21, song_list[audio_song].len);
		res.rem = res.rem / (song_list[audio_song].len / 21);
		res.rem = (res.rem * 5) / 21;
		
		for(i = 0; i < res.quot; i++)
		{
			temp[i] = 0xF5;
		}
		
		if(res.rem > 0)
		{
			temp[i] = 0xF0 + res.rem - 1;
			i++;
		}
		
		temp[i] = 0;
		
		lcd_print(4, 0, temp);
		if(audio_ocr_val_r)
			lcd_print(5, LCD_ATTR_CENTER, "%uHz - %.2fs", (uint16_t)(F_CPU / 2 / audio_ocr_val_r), (double)(nlen * 0.016f));
		else
			lcd_print(5, LCD_ATTR_CENTER, "Pause - %.2fs", (double)(nlen * 0.016f));
	}	
	
	if(audio_ocr_val_r)
	{
		if(isset(audio_flags, FLAG_BEAT_10))
		{
			audio_ocr_val_l -= noten_ocr_binaural_mod[npos];
			audio_ocr_val_r += noten_ocr_binaural_mod[npos];
		}
		if(isset(audio_flags, FLAG_BEAT_20))
		{
			audio_ocr_val_l -= (noten_ocr_binaural_mod[npos] / 2);
			audio_ocr_val_r += (noten_ocr_binaural_mod[npos] / 2);
		}
		SPEAKER_ON;
	}	
		
	audio_pos++;
}

// stop
void audio_stop(void)
{
	AUDIO_OFF;
	SPEAKER_OFF;
	cbi(audio_flags, FLAG_ISPLAYING | FLAG_AUDIO_MENU | FLAG_RANDOM);
}

// lied abspielen
void audio_play(uint8_t l, uint8_t rep, uint8_t binbeat)
{
	if(l >= SONG_NUM)
		return;
	
	cbi(audio_flags, FLAG_ISPLAYING);
	audio_pos = 0;
	audio_song = l;
	sbi(audio_flags, FLAG_ISPLAYING);
	
	if(rep)
		sbi(audio_flags, FLAG_REPEAT);
	else
		cbi(audio_flags, FLAG_REPEAT);
		
	cbi(audio_flags, FLAG_BEAT_10 | FLAG_BEAT_20);
	if(binbeat == 1)
		sbi(audio_flags, FLAG_BEAT_10);
	else if(binbeat == 2)
		sbi(audio_flags, FLAG_BEAT_20);
	
	AUDIO_ON;
}

void audio_play_random(void)
{
	cbi(audio_flags, FLAG_BEAT_10 | FLAG_BEAT_20);
	sbi(audio_flags, FLAG_RANDOM | FLAG_ISPLAYING | FLAG_REPEAT);
	
	audio_pos = 0;
	audio_song = rand() % SONG_NUM;
	
	AUDIO_ON;
}

// init
void audio_init(void)
{
	audio_flags = 0;
	AUDIO_OFF;
	SPEAKER_OFF;
}

// interface für jukebox menü
uint8_t audio_menu_pos;

void audio_menuStart(void)
{
	audio_menu_pos = 0;
	audio_stop();
	uil_startFunc();
	audio_menuDraw();
	sbi(audio_flags, FLAG_AUDIO_MENU);
}

void audio_menuDraw(void)
{
	if(isset(audio_flags, FLAG_ISPLAYING))		// status
	{
		lcd_clr();
		
		lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Jukebox");
		lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE8 zur\x81""ck");
		lcd_print(2, LCD_ATTR_CENTER, song_list[audio_menu_pos].name);
	}
	else			// liste
	{
		uint8_t start, i = 0;
		
		lcd_clr();
		lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Jukebox");
		lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE7 Start \xE8 zur\x81""ck");
		
		start = (audio_menu_pos / 6) * 6;
		
		for(i = 0; i < 6 && start + i < SONG_NUM; i++)
		{
			if(start + i == audio_menu_pos)
			{
				lcd_print(1 + i, LCD_ATTR_INVERT_T, song_list[start + i].name);
			}
			else
			{
				lcd_print(1 + i, 0, song_list[start + i].name);
			}
		}
	}
}

void audio_menuInput(void)
{
	if(isset(audio_flags, FLAG_ISPLAYING))
	{
		if(ps2_data.btn2_down & PS2_CIRCLE)
		{
			audio_stop();
			sbi(audio_flags, FLAG_AUDIO_MENU);
			audio_menuDraw();
		}
	}
	else
	{
		if(ps2_data.btn1_down & PS2_UP)		// hoch
		{
			if(audio_menu_pos > 0)
			{
				audio_menu_pos--;
				audio_menuDraw();
			}
		}
		else if(ps2_data.btn1_down & PS2_DOWN)		// runter
		{
			if(audio_menu_pos < SONG_NUM - 1)
			{
				audio_menu_pos++;
				audio_menuDraw();
			}
		}
		
		if(ps2_data.btn2_down & PS2_CROSS)		// bestätigen/auswählen
		{
			audio_play(audio_menu_pos, 0, 0);
			anim_jukebox_start();
			audio_menuDraw();
		}
		
		if(ps2_data.btn2_down & PS2_SQUARE)		// 10Hz
		{
			audio_play(audio_menu_pos, 0, 1);
			audio_menuDraw();
		}
		if(ps2_data.btn2_down & PS2_TRIANGLE)		// 20Hz
		{
			audio_play(audio_menu_pos, 0, 2);
			audio_menuDraw();
		}
		
		if(ps2_data.btn2_down & PS2_CIRCLE)		// abbrechen/zurück
		{
			cbi(audio_flags, FLAG_AUDIO_MENU);
			anim_setRandom(1);
			anim_random(ANIM_PLAY_LEN);
			uil_endFunc();
		}	
	}
}