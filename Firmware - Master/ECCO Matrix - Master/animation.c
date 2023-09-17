/*
	LED Animationen
*/ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "master.h"
#include "animation.h"
#include "led.h"
#include "ui_lcd.h"
#include "ps2controller.h"
#include "lcd.h"
#include "audio.h"
//#include "audio_lieder.h"

#define random_color			(rand() % LED_COL_MAX)
#define random_color_h			(10 + (rand() % (LED_COL_MAX - 10)))

// reservierter speicher für animationen
uint8_t anim_data_buffer[512];

uint8_t anim_flags;

// anzahl animationen
#define ANIM_NUM		10
anim_list_t anim_list[ANIM_NUM] = {
	{NULL, anim_regen_step, 5, "Regen"},
	{anim_regen2_init, anim_regen2_step, 1, "Regen 2"},
	{anim_fader1_init, anim_fader1_step, 0, "Fader 1"},
	{NULL, anim_disco_step, 1, "Disco"},
	{anim_regenbogen_init, anim_regenbogen_step, 0, "Regenbogen"},
	{anim_armding_init, anim_armding_step, 0, "Regenbogenrad"},
	{anim_linie_init, anim_linie_step, 3, "Linien"},
	{anim_mpixel_init, anim_mpixel_step, 2, "Stern"},
	{anim_pfeile_init, anim_pfeile_step, 6, "Pfeile"},
	{NULL, anim_jukebox_step, 0, "NICHT SICHTBAR"}			// muss letzte element sein
};

// timestamp wann wechsel zur nächsten
volatile uint32_t anim_next_ts = 0;

// momentane animation
uint8_t anim_cur = 0;

typedef enum {RECHTS = 0, RUNTER, LINKS, HOCH} richtung_t;

/*
	interface für menü
*/
uint8_t app_MenuPos;
void anim_menuDraw(void)
{
	uint8_t start, i = 0, t;
	
	lcd_clr();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Animationen");
	lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE7 Start \xE8 zur\x81""ck");
	
	start = (app_MenuPos / 6) * 6;
	
	for(i = 0; i < 6 && start + i < (ANIM_NUM - 1); i++)
	{
		if(start + i == anim_cur)
			t = LCD_ATTR_CENTER;
		else
			t = 0;
			
		if(start + i == app_MenuPos)
		{
			lcd_print(1 + i, LCD_ATTR_INVERT_T | t, anim_list[start + i].name);
		}
		else
		{
			lcd_print(1 + i, t, anim_list[start + i].name);
		}
	}
}

void anim_menuStart(void)
{
	uil_startFunc();
	anim_setRandom(0);
	app_MenuPos = 0;
	
	anim_menuDraw();
}

void anim_menuInput(void)
{
	if(ps2_data.btn1_down & PS2_UP)		// hoch
	{
		if(app_MenuPos > 0)
		{
			app_MenuPos--;
			anim_menuDraw();
		}
	}
	else if(ps2_data.btn1_down & PS2_DOWN)		// runter
	{
		if(app_MenuPos < ANIM_NUM - 2)
		{
			app_MenuPos++;
			anim_menuDraw();
		}
	}
		
	if(ps2_data.btn2_down & PS2_CROSS)		// bestätigen/auswählen
	{
		anim_play(app_MenuPos);
		anim_menuDraw();
	}
		
	if(ps2_data.btn2_down & PS2_CIRCLE)		// abbrechen/zurück
	{
		anim_setRandom(1);
		uil_endFunc();
	}		
}

/*
	Grundsystem
*/

// init
void anim_init(void)
{
	anim_cur = 0;
	anim_next_ts = timestamp + 30000;
	anim_flags = 0;
	sbi(anim_flags, ANIM_FLAG_RUNNING);
	
	if(anim_list[anim_cur].func_init)
		anim_list[anim_cur].func_init();
}

// zufalls animationen ein/aus
void anim_setRandom(uint8_t r)
{
	if(r)
		sbi(anim_flags, ANIM_FLAG_RANDOM);
	else
		cbi(anim_flags, ANIM_FLAG_RANDOM);
}

// jeden frame aufrufen
void anim_frame(void)
{
	static uint8_t step = 0;
	
	if(!isset(anim_flags, ANIM_FLAG_RUNNING))
		return;
	
	if(timestamp > anim_next_ts)		// neue animation
	{
		if(!isset(anim_flags, ANIM_FLAG_RANDOM))
		{
			anim_next_ts = timestamp + ANIM_PLAY_LEN * 1000UL;
			return;
		}
		cbi(anim_flags, ANIM_FLAG_RUNNING);
		anim_random(ANIM_PLAY_LEN);
		step = 0;
		sbi(anim_flags, ANIM_FLAG_RUNNING);
	}
	else								// weiter
	{
		if(step >= anim_list[anim_cur].step_delay)
		{
			anim_list[anim_cur].func_step();
			step = 0;
		}
		else
		{
			step++;
		}
	}
}

// animation num abspielen
void anim_play(uint8_t num)
{
	if(num >= ANIM_NUM)
		return;
	
	cbi(anim_flags, ANIM_FLAG_RANDOM);
	cbi(anim_flags, ANIM_FLAG_RUNNING);
	anim_cur = num;
	
	if(anim_list[anim_cur].func_init)
		anim_list[anim_cur].func_init();
	sbi(anim_flags, ANIM_FLAG_RUNNING);
}

// zufällige neue animation
void anim_random(uint32_t sec)
{
	uint8_t anim_next;
	
	do 
	{
		anim_next = rand() % (ANIM_NUM - 1);
	} while(anim_next == anim_cur);
	
	anim_cur = anim_next;
	anim_next_ts = timestamp + (uint32_t)(sec * 1000);
	
	if(anim_list[anim_cur].func_init)
		anim_list[anim_cur].func_init();
}

/*
	Hilfskram
*/

// alle farben auf 0
void anim_clear_all(void)
{
	uint8_t i, j;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			led_data_buffer[i][j].r = 0;
			led_data_buffer[i][j].g = 0;
			led_data_buffer[i][j].b = 0;
		}
	}
}

/*
	Animationen
*/
/*
// ECCO startup schriftzug
typedef struct {
	uint8_t pos;
	uint16_t data[5];
	led_data_t col;
} anim_ecco_t;

void anim_ecco_init(void)
{
	anim_ecco_t *d = (anim_ecco_t*)anim_data_buffer;
	
	d->pos = 15;
	d->data[0] = 0xEEEE;	// 0b1110111011101110;
	d->data[1] = 0x888A;	// 0b1000100010001010;
	d->data[2] = 0xE88A; 	// 0b1110100010001010;
	d->data[3] = 0x888A;	// 0b1000100010001010;
	d->data[4] = 0xEEEE;	// 0b1110111011101110;
	
	d->col.r = random_color_h;
	d->col.g = random_color_h;
	d->col.b = random_color_h;
}

void anim_ecco_step(void)
{
	uint8_t y;
	uint16_t mask;
	anim_ecco_t *d = (anim_ecco_t*)anim_data_buffer;
	
	led_shiftLeft();
	
	mask = (1 << d->pos);
	
	for(y = 0; y < 5; y++)
	{
		if(d->data[y] & mask)
		{
			led_data_buffer[4][y].r = d->col.r;
			led_data_buffer[4][y].g = d->col.g;
			led_data_buffer[4][y].b = d->col.b;
		}
		else
		{
			led_data_buffer[4][y].r = 0;
			led_data_buffer[4][y].g = 0;
			led_data_buffer[4][y].b = 0;
		}
	}
	
	d->pos--;
	
	if(d->pos > 16)
	{
		d->pos = 15;
		
		led_getCol(&d->col);
	}
}
*/
//

void anim_regen_step(void)
{
	uint8_t i;
	
	led_shiftDown();
	
	for(i = 0; i < 5; i++)
	{
		if(rand() % 100 > 75)
		{
			led_getCol(&led_data_buffer[i][0]);
		}
		else
		{
			led_data_buffer[i][0].r = 0;
			led_data_buffer[i][0].g = 0;
			led_data_buffer[i][0].b = 0;
		}
	}
}

// fader - jedes feld für sich
typedef struct {
	uint8_t step[5][5][3], dir[5][5][3], cnt[5][5][3], pause[5][5][3];
	uint8_t counter;
} anim_fader1_t;

void anim_fader1_init(void)
{
	anim_fader1_t *d = (anim_fader1_t*)anim_data_buffer;
	uint8_t x, y;
	led_data_t col;
	
	d->counter = 0;
	
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			col.r = random_color;
			col.g = random_color;
			col.b = random_color;
			
			if(led_data_buffer[x][y].r < col.r)
				d->dir[x][y][0] = 1;
			else
				d->dir[x][y][0] = 0;
			
			if(led_data_buffer[x][y].g < col.g)
				d->dir[x][y][1] = 1;
			else
				d->dir[x][y][1] = 0;
			
			if(led_data_buffer[x][y].b < col.b)
				d->dir[x][y][2] = 1;
			else
				d->dir[x][y][2] = 0;
			
			d->step[x][y][0] = abs(led_data_buffer[x][y].r - col.r) + 1;
			d->step[x][y][1] = abs(led_data_buffer[x][y].g - col.g) + 1;
			d->step[x][y][2] = abs(led_data_buffer[x][y].b - col.b) + 1;
			
			d->cnt[x][y][0] = rand() % 5;
			d->cnt[x][y][1] = rand() % 5;
			d->cnt[x][y][2] = rand() % 5;
			
			d->pause[x][y][0] = 0;
			d->pause[x][y][1] = 0;
			d->pause[x][y][2] = 0;
		}
	}
}

void anim_fader1_step(void)
{
	anim_fader1_t *d = (anim_fader1_t*)anim_data_buffer;
	uint8_t x, y, temp;
	
	d->counter++;
	
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			// r
			if(d->pause[x][y][0] == 0)	// keine pause
			{
				if(d->counter % d->cnt[x][y][0] == 0)	// benötigt update
				{
					if(d->dir[x][y][0])	// hoch
					{
						led_data_buffer[x][y].r++;
						if(led_data_buffer[x][y].r > LED_COL_MAX)
							led_data_buffer[x][y].r = LED_COL_MAX;
					}
					else	// runter
					{
						led_data_buffer[x][y].r--;
						if(led_data_buffer[x][y].r > LED_COL_MAX)
							led_data_buffer[x][y].r = 0;
					}
					d->step[x][y][0]--;
					
					if(d->step[x][y][0] == 0)	// ziel erreicht
					{
						d->pause[x][y][0] = rand() % 50 + 10;
						
						// neuer wert
						temp = random_color;
						
						if(led_data_buffer[x][y].r < temp)
							d->dir[x][y][0] = 1;
						else
							d->dir[x][y][0] = 0;
						d->step[x][y][0] = abs(led_data_buffer[x][y].r - temp) + 1;
						d->cnt[x][y][0] = rand() % 5;
					}
				}
			}
			else
			{
				d->pause[x][y][0]--;
			} // r
			
			// g
			if(d->pause[x][y][1] == 0)	// keine pause
			{
				if(d->counter % d->cnt[x][y][1] == 0)	// benötigt update
				{
					if(d->dir[x][y][1])	// hoch
					{
						led_data_buffer[x][y].g++;
						if(led_data_buffer[x][y].g > LED_COL_MAX)
							led_data_buffer[x][y].g = LED_COL_MAX;
					}
					else	// runter
					{
						led_data_buffer[x][y].g--;
						if(led_data_buffer[x][y].g > LED_COL_MAX)
							led_data_buffer[x][y].g = 0;
					}
					d->step[x][y][1]--;
					
					if(d->step[x][y][1] == 0)	// ziel erreicht
					{
						d->pause[x][y][1] = rand() % 50 + 10;
						
						// neuer wert
						temp = random_color;
						
						if(led_data_buffer[x][y].g < temp)
							d->dir[x][y][1] = 1;
						else
							d->dir[x][y][1] = 0;
						d->step[x][y][1] = abs(led_data_buffer[x][y].g - temp) + 1;
						d->cnt[x][y][1] = rand() % 5;
					}
				}
			}
			else
			{
				d->pause[x][y][1]--;
			} // g
			
			// b
			if(d->pause[x][y][2] == 0)	// keine pause
			{
				if(d->counter % d->cnt[x][y][2] == 0)	// benötigt update
				{
					if(d->dir[x][y][2])	// hoch
					{
						led_data_buffer[x][y].b++;
						if(led_data_buffer[x][y].b > LED_COL_MAX)
							led_data_buffer[x][y].b = LED_COL_MAX;
					}
					else	// runter
					{
						led_data_buffer[x][y].b--;
						if(led_data_buffer[x][y].b > LED_COL_MAX)
							led_data_buffer[x][y].b = 0;
					}
					d->step[x][y][2]--;
					
					if(d->step[x][y][2] == 0)	// ziel erreicht
					{
						d->pause[x][y][2] = rand() % 50 + 10;
						
						// neuer wert
						temp = random_color;
						
						if(led_data_buffer[x][y].b < temp)
							d->dir[x][y][2] = 1;
						else
							d->dir[x][y][2] = 0;
						d->step[x][y][2] = abs(led_data_buffer[x][y].b - temp) + 1;
						d->cnt[x][y][2] = rand() % 5;
					}
				}
			}
			else
			{
				d->pause[x][y][2]--;
			} // b
		}
	}
}

// regen mit spur

#define REGEN2_TROPFEN_NUM		8
typedef struct {
	struct {
		uint8_t x, y;
		led_data_t col;
	} tropfen[REGEN2_TROPFEN_NUM];
	uint8_t num;	
} anim_regen2_t;

void anim_regen2_init(void)
{
	anim_regen2_t *d = (anim_regen2_t*)anim_data_buffer;
	uint8_t i;
	
	d->num = 0;
	for(i = 0; i < REGEN2_TROPFEN_NUM; i++)
	{
		d->tropfen[i].x = 0xFF;			// als unbenutzt markieren
	}
}

void anim_regen2_step(void)
{	
	anim_regen2_t *d = (anim_regen2_t*)anim_data_buffer;
	uint8_t i, x, y, pos;
	
	if(d->num < REGEN2_TROPFEN_NUM - 1)		// platz für neuen tropfen
	{
		if(rand() % 100 > 60)
		{
			pos = 0;
			for(i = 0; i < REGEN2_TROPFEN_NUM; i++)
			{
				if(d->tropfen[i].x == 0xFF)
				{
					pos = i;
					break;
				}
			}
			
			d->tropfen[pos].y = 0;
			d->tropfen[pos].x = rand() % 5;
			led_getCol(&d->tropfen[pos].col);
			
			d->num++;
		}
	}
	
	// alles ausfaden
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			if(led_data_buffer[x][y].r)
				led_data_buffer[x][y].r--;
				
			if(led_data_buffer[x][y].g)
				led_data_buffer[x][y].g--;
				
			if(led_data_buffer[x][y].b)
				led_data_buffer[x][y].b--;
		}
	}
	
	// tropfen reinsetzen und bewegen
	for(i = 0; i < REGEN2_TROPFEN_NUM; i++)
	{
		if(d->tropfen[i].x != 0xFF)
		{
			led_data_buffer[d->tropfen[i].x][d->tropfen[i].y].r |= d->tropfen[i].col.r;
			led_data_buffer[d->tropfen[i].x][d->tropfen[i].y].g |= d->tropfen[i].col.g;
			led_data_buffer[d->tropfen[i].x][d->tropfen[i].y].b |= d->tropfen[i].col.b;
			
			d->tropfen[i].y++;
			if(d->tropfen[i].y >= 5)
			{
				d->tropfen[i].x = 0xFF;
				d->num--;
			}
		}
	}
}

// aufleuchten + faden
void anim_disco_step(void)
{
	uint8_t x, y;
	
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			if(rand() % 100 > 80 && (led_data_buffer[x][y].r < (LED_COL_MAX / 2) && led_data_buffer[x][y].g < (LED_COL_MAX / 2) && led_data_buffer[x][y].b < (LED_COL_MAX / 2)))		// neues feld
			{
				led_getCol(&led_data_buffer[x][y]);
			}
			else		// kein neues
			{
				if(led_data_buffer[x][y].r)
					led_data_buffer[x][y].r--;
				
				if(led_data_buffer[x][y].g)
					led_data_buffer[x][y].g--;
				
				if(led_data_buffer[x][y].b)
					led_data_buffer[x][y].b--;
			}
		}
	}
}

// rotierendes arm ding
#define ANIM_ARMDING_CNT		1
typedef struct {
	uint8_t x, y;					// position aussen
	richtung_t dir;
	
	led_data_t col;					// farbe
	uint8_t state;				//
	
	uint8_t cnt;					// prescaler für armbewegung
} anim_arm_t;

void anim_armding_init(void)
{
	anim_arm_t *d = (anim_arm_t*)anim_data_buffer;
	
	d->x = rand() % 4;
	d->y = 0;
	d->dir = RECHTS;
	
	d->col.r = LED_COL_MAX;
	d->col.g = 0;
	d->col.b = 0;
	
	d->state = 0;
	
	d->cnt = ANIM_ARMDING_CNT;
}

void anim_armding_step(void)
{
	uint8_t tx, ty;
	anim_arm_t *d = (anim_arm_t*)anim_data_buffer;

	// arm rein
	led_data_buffer[2][2].r = d->col.r;
	led_data_buffer[2][2].g = d->col.g;
	led_data_buffer[2][2].b = d->col.b;
			
	led_data_buffer[d->x][d->y].r = d->col.r;
	led_data_buffer[d->x][d->y].g = d->col.g;
	led_data_buffer[d->x][d->y].b = d->col.b;
			
	if(d->x == 0)
	{
		tx = 1;
		ty = 1 + ((d->y + 1) / 2);
	}
	else if(d->y == 0)
	{
		ty = 1;
		tx = 1 + (d->x / 2);
	}
	else if(d->x == 4)
	{
		tx = 3;
		ty = 1 + (d->y / 2);
	}
	else if(d->y == 4)
	{
		ty = 3;
		tx = 1 + ((d->x + 1) / 2);
	}
	else
	{
		anim_armding_init();
		return;
	}		
			
	led_data_buffer[tx][ty].r = d->col.r;
	led_data_buffer[tx][ty].g = d->col.g;
	led_data_buffer[tx][ty].b = d->col.b;
	
	// arm bewegen	
	if(d->cnt)
	{
		d->cnt--;
	}
	else
	{
		switch(d->dir)
		{
		case RECHTS:
			d->x++;
			if(d->x == 4)
				d->dir = RUNTER;
			break;
				
		case RUNTER:
			d->y++;
			if(d->y == 4)
				d->dir = LINKS;
			break;
				
		case LINKS:
			d->x--;
			if(d->x == 0)
				d->dir = HOCH;
			break;
				
		case HOCH:
			d->y--;
			if(d->y == 0)
				d->dir = RECHTS;
			break;
			
		default:
			anim_armding_init();
			return;
		}
		d->cnt = ANIM_ARMDING_CNT;				
	}
	
	// arm faden
	switch(d->state)
	{
	case 0:		// g++;
		if(d->col.g < LED_COL_MAX)
		{
			d->col.g++;
		}
		else
		{
			d->state++;
		}
		break;
		
	case 1:		// r--;
		if(d->col.r > 0)
		{
			d->col.r--;
		}
		else
		{
			d->state++;
		}
		break;
		
	case 2:		// b++
		if(d->col.b < LED_COL_MAX)
		{
			d->col.b++;
		}
		else
		{
			d->state++;
		}
		break;
		
	case 3:		// g--
		if(d->col.g > 0)
		{
			d->col.g--;
		}
		else
		{
			d->state++;
		}
		break;
		
	case 4:		// r++
		if(d->col.r < LED_COL_MAX)
		{
			d->col.r++;
		}
		else
		{
			d->state++;
		}
		break;
		
	case 5:		// b--
		if(d->col.b > 0)
		{
			d->col.b--;
		}
		else
		{
			d->state = 0;
		}
		break;
		
	default:
		d->col.r = LED_COL_MAX;
		d->col.g = 0;
		d->col.b = 0;
		d->state = 0;
	}
}

// linien
#define ANIM_LINIEN_MAX		2

// mischen:
// 0 - ersetzen
// 1 - or
// 2 - xor
// 3 - and

// step
// 0 - runter//links
// 1 - gerade
// 2 - hoch//rechts

typedef struct {
	uint8_t num;
	struct {
		uint8_t x, y, sx, sy;
		led_data_t col;
	} linie[ANIM_LINIEN_MAX];
} anim_linien_t;

void anim_linie_init(void)
{
	anim_linien_t *d = (anim_linien_t*)anim_data_buffer;
	uint8_t i;
	
	d->num = 0;
	for(i = 0; i < ANIM_LINIEN_MAX; i++)
	{
		d->linie[i].sx = 0xFF;		// unbenutzt
	}
}

void anim_linie_step(void)
{
	anim_linien_t *d = (anim_linien_t*)anim_data_buffer;
	uint8_t i, pos = 0;
	
	for(i = 0; i < ANIM_LINIEN_MAX; i++)
	{
		if(d->linie[i].sx == 0xFF)	// unbenutzt
		{	
			pos = i;
			continue;
		}
		
		led_data_buffer[d->linie[i].x][d->linie[i].y].r = d->linie[i].col.r;
		led_data_buffer[d->linie[i].x][d->linie[i].y].g = d->linie[i].col.g;
		led_data_buffer[d->linie[i].x][d->linie[i].y].b = d->linie[i].col.b;
		
		if(d->linie[i].sx == 0)
		{
			d->linie[i].x--;
		}
		else if(d->linie[i].sx == 2)
		{
			d->linie[i].x++;
		}
		
		if(d->linie[i].sy == 0)
		{
			d->linie[i].y++;
		}
		else if(d->linie[i].sy == 2)
		{
			d->linie[i].y--;
		}
		
		if(d->linie[i].y > 4 || d->linie[i].x > 4)	// leeren
		{
			d->num--;
			d->linie[i].sx = 0xFF;
		}
	}
	
	if(d->linie[pos].sx == 0xFF)		// noch platz für neue
	{
		if(rand() % 100 > 50)	// neue einfügen
		{
			i = rand() % 4;
			
			switch(i)
			{
			case 0:	// start links
				d->linie[pos].x = 0;
				d->linie[pos].y = rand() % 5;
				d->linie[pos].sx = 2;
				d->linie[pos].sy = rand() % 3;					
				break;
			
			case 1:	// start rechts
				d->linie[pos].x = 4;
				d->linie[pos].y = rand() % 5;
				d->linie[pos].sx = 0;
				d->linie[pos].sy = rand() % 3;
				break;
				
			case 2:	// start oben
				d->linie[pos].x = rand() % 5;
				d->linie[pos].y = 0;
				d->linie[pos].sy = 0;
				d->linie[pos].sx = rand() % 3;
				break;
				
			case 3:	// start unten
				d->linie[pos].x = rand() % 5;
				d->linie[pos].y = 4;
				d->linie[pos].sy = 2;
				d->linie[pos].sx = rand() % 3;
				break;
			} // switch
			d->linie[pos].col.r = random_color;
			d->linie[pos].col.g = random_color;
			d->linie[pos].col.b = random_color;
			d->num++;
		} // if neu
	} // if platz
}

// pixel aus mitte
#define ANIM_MPIXEL_MAX		8

typedef struct {
	struct {
		uint8_t x, y, sx, sy;
		led_data_t col;
	} pixel[ANIM_MPIXEL_MAX];
} anim_mpixel_t;

void anim_mpixel_init(void)
{
	anim_mpixel_t *d = (anim_mpixel_t*)anim_data_buffer;
	uint8_t i;
	
	for(i = 0; i < ANIM_MPIXEL_MAX; i++)
	{
		d->pixel[i].sx = 0xFF;		// leer
	}
}

void anim_mpixel_step(void)
{
	anim_mpixel_t *d = (anim_mpixel_t*)anim_data_buffer;
	uint8_t i, j;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			if(led_data_buffer[i][j].r)
				led_data_buffer[i][j].r--;
			if(led_data_buffer[i][j].g)
				led_data_buffer[i][j].g--;
			if(led_data_buffer[i][j].b)
				led_data_buffer[i][j].b--;
		}
	}
	
	for(i = 0; i < ANIM_MPIXEL_MAX; i++)
	{
		if(d->pixel[i].sx != 0xFF)		// aktiv
		{
			
			led_data_buffer[d->pixel[i].x][d->pixel[i].y].r = d->pixel[i].col.r;
			led_data_buffer[d->pixel[i].x][d->pixel[i].y].g = d->pixel[i].col.g;
			led_data_buffer[d->pixel[i].x][d->pixel[i].y].b = d->pixel[i].col.b;
			
			if(d->pixel[i].sx == 0)
			{
				d->pixel[i].x--;
			}
			else if(d->pixel[i].sx == 2)
			{
				d->pixel[i].x++;
			}
			
			if(d->pixel[i].sy == 0)
			{
				d->pixel[i].y--;
			}
			else if(d->pixel[i].sy == 2)
			{
				d->pixel[i].y++;
			}
			
			if(d->pixel[i].x > 4 || d->pixel[i].y > 4)
			{
				d->pixel[i].sx = 0xFF;
			}
		}
		else			// inaktiv
		{
			if(rand() % 100 > 60)
			{
				d->pixel[i].x = 2;
				d->pixel[i].y = 2;
				d->pixel[i].col.r = random_color;
				d->pixel[i].col.g = random_color;
				d->pixel[i].col.b = random_color;
				do {
					d->pixel[i].sx = rand() % 3;
					d->pixel[i].sy = rand() % 3;
				} while(d->pixel[i].sx == 1 && d->pixel[i].sy == 1);			
			}				
		}
	
	} // for pixel
}

// jukebox animation
extern lied_t song_list[SONG_NUM];
extern uint16_t noten_ocr_val[NOTEN_NUM];
extern uint16_t audio_pos;
extern uint8_t audio_song;

#define IST_DELAY		3		// IST hoch
#define SOLL_DELAY		5		// SOLL runter

typedef struct {
	uint16_t min, max, limit[5];
	uint8_t ist[5], soll[5];
	led_data_t col[5];
	uint8_t ist_cnt, soll_cnt;
} anim_jukebox_t;

void anim_jukebox_start(void)
{
	anim_jukebox_t *d = (anim_jukebox_t*)anim_data_buffer;
	uint16_t i, temp;
	
	cbi(anim_flags, ANIM_FLAG_RUNNING);
	anim_setRandom(0);
	
	d->min = 0xFFFF;
	d->max = 0;
	d->ist_cnt = IST_DELAY;
	d->soll_cnt = SOLL_DELAY;
	
	for(i = 0; i < song_list[audio_song].len; i++)
	{
		temp = noten_ocr_val[pgm_read_byte(&song_list[audio_song].noten[i].note)];
		if(temp == 0)
			continue;
		if(temp < d->min)
			d->min = temp;
		if(temp > d->max)
			d->max = temp;
	}
	
	d->limit[0] = d->min;
	for(i = 1; i < 5; i++)
	{
		d->limit[i] = (uint16_t)(((uint32_t)i * ((uint32_t)d->max - (uint32_t)d->min)) / 5);
		d->limit[i] += d->min;
	}

	for(i = 0; i < 5; i++)
	{
		d->soll[i] = 0;
		d->ist[i] = 0;
		
		d->col[i].r = 0;
		d->col[i].g = 0;
		d->col[i].b = 0;
	}
	
	d->col[0].r = LED_COL_MAX;
	
	d->col[1].r = LED_COL_MAX;
	d->col[1].g = LED_COL_MAX;
	
	d->col[2].g = LED_COL_MAX;
	
	d->col[3].g = LED_COL_MAX;
	d->col[3].b = LED_COL_MAX;
	
	d->col[4].b = LED_COL_MAX;

	anim_cur = ANIM_NUM - 1;
	anim_next_ts = timestamp + 300000UL;
	sbi(anim_flags, ANIM_FLAG_RUNNING);
}

void anim_jukebox_step(void)
{
	anim_jukebox_t *d = (anim_jukebox_t*)anim_data_buffer;
	uint8_t pos, i, j;
	uint16_t temp;

	if(audio_pos == 0)
		return;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			if(led_data_buffer[i][j].r > 2)
				led_data_buffer[i][j].r -= 2;
			else
				led_data_buffer[i][j].r = 0;
				
			if(led_data_buffer[i][j].g > 2)
				led_data_buffer[i][j].g -= 2;
			else
				led_data_buffer[i][j].g = 0;
				
			if(led_data_buffer[i][j].b > 2)
				led_data_buffer[i][j].b -= 2;
			else
				led_data_buffer[i][j].b = 0;
		}
	}	
	
	for(i = 0; i < 5; i++)
	{
		led_data_buffer[i][4].r = d->col[i].r;
		led_data_buffer[i][4].g = d->col[i].g;
		led_data_buffer[i][4].b = d->col[i].b;
	}				
	
	temp = noten_ocr_val[pgm_read_byte(&song_list[audio_song].noten[audio_pos - 1].note)];
	if(temp == 0)
	{
		for(i = 0; i < 5; i++)
		{
			if(d->soll[i])
			{
				d->soll[i]--;
			}
			if(d->ist[i])
			{
				d->ist[i]--;
			}
		}
		return;
	}	
	
	if(temp >= d->limit[0] && temp < d->limit[1])
	{
		pos = 4;
	}
	else if(temp >= d->limit[1] && temp < d->limit[2])
	{
		pos = 3;
	}
	else if(temp >= d->limit[2] && temp < d->limit[3])
	{
		pos = 2;
	}
	else if(temp >= d->limit[3] && temp < d->limit[4])
	{
		pos = 1;
	}
	else
	{
		pos = 0;
	}
	
	if(!--d->soll_cnt)
	{
		for(i = 0; i < 5; i++)
		{
			if(d->soll[i])
				d->soll[i]--;
		}
		
		d->soll_cnt = SOLL_DELAY;
	}
	
	if(d->soll[pos] < 4)
	{
		d->soll[pos] += 1;
		if(d->soll[pos] > 4)
			d->soll[pos] = 4;
	}		

	if(pos > 0)
	{
		if(d->soll[pos - 1] < 2)
			d->soll[pos - 1]++;
	}
	
	if(pos < 4)
	{
		if(d->soll[pos + 1] < 2)
			d->soll[pos + 1]++;
	}		
		
	if(!--d->ist_cnt)
	{
		for(i = 0; i < 5; i++)
		{
			if(d->ist[i] < d->soll[i])
				d->ist[i]++;
			else if(d->ist[i] > d->soll[i])
				d->ist[i]--;
		}
		
		d->ist_cnt = IST_DELAY;
	}
	
	if(audio_pos < song_list[audio_song].len)
	{
		for(i = 0; i < 5; i++)
		{
//			led_data_buffer[i][4].r = d->col[i].r;
//			led_data_buffer[i][4].g = d->col[i].g;
//			led_data_buffer[i][4].b = d->col[i].b;
			for(j = 1; j <= d->ist[i]; j++)
			{
				led_data_buffer[i][4 - j].r = d->col[i].r;
				led_data_buffer[i][4 - j].g = d->col[i].g;
				led_data_buffer[i][4 - j].b = d->col[i].b;
			}
		}
	}
}

// pfeile
typedef struct {
	led_data_t col[3];
	uint8_t cnt;
	richtung_t dir;
} anim_pfeile_t;

void anim_pfeile_init(void)
{
	anim_pfeile_t *d = (anim_pfeile_t*)anim_data_buffer;
	uint8_t i, n;
	
	d->cnt = 30 + (rand() % 30);
	
	do {
		n = rand() % 4;
	} while(d->dir == n);
	d->dir = n;
	
	for(i = 0; i < 3; i++)
	{
		d->col[i].r = random_color;
		d->col[i].g = random_color;
		d->col[i].b = random_color;
	}
}

void anim_pfeile_step(void)
{
	anim_pfeile_t *d = (anim_pfeile_t*)anim_data_buffer;

	// alles 1 in richtung und neue farbe setzen
	switch(d->dir)
	{
	case HOCH:
		led_shiftUp();
		
		led_copyCol(&led_data_buffer[0][4], &d->col[0]);
		led_copyCol(&led_data_buffer[4][4], &d->col[0]);
		
		led_copyCol(&led_data_buffer[1][4], &d->col[1]);
		led_copyCol(&led_data_buffer[3][4], &d->col[1]);
		
		led_copyCol(&led_data_buffer[2][4], &d->col[2]);
		break;

	case RUNTER:
		led_shiftDown();

		led_copyCol(&led_data_buffer[0][0], &d->col[0]);
		led_copyCol(&led_data_buffer[4][0], &d->col[0]);
		
		led_copyCol(&led_data_buffer[1][0], &d->col[1]);
		led_copyCol(&led_data_buffer[3][0], &d->col[1]);
		
		led_copyCol(&led_data_buffer[2][0], &d->col[2]);
		break;
		
	case LINKS:
		led_shiftLeft();

		led_copyCol(&led_data_buffer[4][0], &d->col[0]);
		led_copyCol(&led_data_buffer[4][4], &d->col[0]);
		
		led_copyCol(&led_data_buffer[4][1], &d->col[1]);
		led_copyCol(&led_data_buffer[4][3], &d->col[1]);
		
		led_copyCol(&led_data_buffer[4][2], &d->col[2]);
		break;
	
	case RECHTS:
		led_shiftRight();

		led_copyCol(&led_data_buffer[0][0], &d->col[0]);
		led_copyCol(&led_data_buffer[0][4], &d->col[0]);
		
		led_copyCol(&led_data_buffer[0][1], &d->col[1]);
		led_copyCol(&led_data_buffer[0][3], &d->col[1]);
		
		led_copyCol(&led_data_buffer[0][2], &d->col[2]);
		
		break;
	}
	
	// farben weiterrücken
	led_copyCol(&d->col[0], &d->col[1]);
	led_copyCol(&d->col[1], &d->col[2]);
	
	if(d->col[1].r == 0 && d->col[1].g == 0 && d->col[1].b == 0)
	{
		d->col[2].r = random_color_h;
		d->col[2].g = random_color_h;
		d->col[2].b = random_color_h;		
	}
	else
	{
		d->col[2].r = 0;
		d->col[2].g = 0;
		d->col[2].b = 0;
	}		

	
	d->cnt--;
	if(!d->cnt)
	{
		anim_pfeile_init();
	}
}

// regenbogen mit hue
typedef struct {
	uint8_t hue[9];
	uint8_t dir, orient;
} anim_regenbogen_t;

void anim_regenbogen_init(void)
{
	anim_regenbogen_t *d = (anim_regenbogen_t*)anim_data_buffer;
	uint8_t i;
	
	for(i = 0; i < 9; i++)
	{
		d->hue[i] = 21 * i;
	}
	d->dir = rand() % 2;
	d->orient = rand() % 2;
}

void anim_regenbogen_step()
{
	anim_regenbogen_t *d = (anim_regenbogen_t*)anim_data_buffer;
	led_data_t c[9];
	uint8_t i, x, y, offset = 0;
	
	if(d->dir)
	{
		for(i = 0; i < 9; i++)
		{
			if(d->hue[i] < 251)
			{
				d->hue[i]++;
			}
			else
			{
				d->hue[i] = 0;
			}
			
			led_hue(d->hue[i], &c[i]);
		}
	}
	else
	{
		for(i = 0; i < 9; i++)
		{
			if(d->hue[i] > 0)
			{
				d->hue[i]--;
			}
			else
			{
				d->hue[i] = 251;
			}
			
			led_hue(d->hue[i], &c[i]);
		}
	}
	
	if(d->orient)		// start oben rechts
	{
		for(x = 0; x < 5; x++)
		{
			offset = 4 + x;
			for(y = 0; y < 5; y++)
			{
				led_copyCol(&led_data_buffer[x][y], &c[offset - y]);
			}
		}
	}
	else				// start oben links
	{
		for(x = 0; x < 5; x++)
		{
			offset = x;
			for(y = 0; y < 5; y++)
			{
				led_copyCol(&led_data_buffer[x][y], &c[offset + y]);
			}
		}		
	}
}