#include <avr/io.h>
#include <stdlib.h>

#include "app_base.h"
#include "lcd.h"
#include "ps2controller.h"
#include "led.h"
#include "master.h"
#include "audio.h"
#include "app_highscore.h"

#define STATE_INTRO		0
#define STATE_RUN		1
#define STATE_END		2
#define STATE_HS		3

#define ZEIT_BASE			20

typedef struct {
	struct {
		uint8_t x, y;
		led_data_t col;
		uint8_t col_fade;
	}spieler;
	uint32_t punkte, startzeit;
	uint8_t del, del_max, state, level;
	uint8_t lcd_display;		// 0 = einleitung, 1 = highscore
} app_spiel1_t;

void app_spiel1_ende(void)
{
	audio_stop();
	app_end();
}

void app_spiel1_draw_lcd()
{
	lcd_clear();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Spiel1");
	lcd_print(1, 0, "\xEA Bewegen");
	lcd_print(2, 0, "Ziel:");
	lcd_print(3, 0, "1. Rot ausweichen");
	lcd_print(4, 0, "2. Gelb einsammeln");
	lcd_print(6, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xEA HighScore");
	lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xEF Start \xE6 Ende");
}

void app_spiel1_init(void)
{
	app_spiel1_t *d = (app_spiel1_t*)app_data;
	
	d->spieler.x = 0;
	d->spieler.y = 2;
	d->spieler.col.r = LED_COL_MAX;
	d->spieler.col.g = 0;
	d->spieler.col.b = 0;
	d->spieler.col_fade = 0;
	d->punkte = 0;
	d->del = 0;
	d->del_max = ZEIT_BASE;
	d->state = STATE_INTRO;
	d->level = 1;
	
	app_spiel1_draw_lcd();
	d->lcd_display = 0;
	
	led_setRGB(0, 0, 0);
	
	d->startzeit = timestamp;
}

void app_spiel1_input(void)
{
	app_spiel1_t *d = (app_spiel1_t*)app_data;
	
	if(ps2_data.btn2_down & PS2_SQUARE)
	{
		app_spiel1_ende();
		return;
	}
	
	switch(d->state)
	{
	case STATE_INTRO:
		if(ps2_data.btn1_down & (PS2_UP | PS2_DOWN | PS2_LEFT | PS2_RIGHT))
		{
			if(d->lcd_display == 0)
			{
				d->lcd_display = 1;
				app_highscore_print(highscore_spiel1);
			}
			else
			{
				d->lcd_display = 0;
				app_spiel1_draw_lcd();
			}
		}
		
		if(ps2_data.btn1_down & PS2_START)
		{
			lcd_clear();
			lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Spiel1");
			lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE6 Ende");
			d->state = STATE_RUN;
			d->startzeit = timestamp;
			// audio_play(2, 1, 0);
			audio_play_random();
		}
		break;
		
	case STATE_RUN:
		if(ps2_data.btn1_down & PS2_UP)
		{
			if(d->spieler.y > 0)
				d->spieler.y--;
		}
		if(ps2_data.btn1_down & PS2_DOWN)
		{
			if(d->spieler.y < 4)
				d->spieler.y++;
		}
		
		if(ps2_data.btn1_down & PS2_LEFT)
		{
			if(d->spieler.x > 0)
				d->spieler.x--;
		}
		if(ps2_data.btn1_down & PS2_RIGHT)
		{
			if(d->spieler.x < 4)
				d->spieler.x++;
		}
		break;
		
	case STATE_END:
		if((ps2_data.btn1_down & 0x0F) != 0 || ps2_data.btn2_down != 0)
		{
			app_spiel1_init();
		}
		break;
		
	case STATE_HS:
		if(app_highscore_step())
		{
			app_highscore_addHS(highscore_spiel1, d->punkte);
			app_highscore_save_spiel1();
			app_spiel1_init();		
		}
		break;
	}	
}

void app_spiel1_draw(void)
{
	app_spiel1_t *d = (app_spiel1_t*)app_data;
	uint32_t zdiff = (timestamp - d->startzeit);
	uint8_t i, tx;
	
	if(d->state == STATE_RUN)
	{
		lcd_print(4, 0, "Zeit: %.1f", (float)((float)zdiff / 1000.0f));
		
		if(!d->del)
		{
			d->level = (uint8_t)(zdiff / 10000UL) + 1;
			lcd_print(2, 0, "Level: %d", d->level);
			if(d->level >= ZEIT_BASE)
			{
				d->del_max = 0;
			}
			else
			{
				d->del_max = ZEIT_BASE - d->level;
			}
			d->punkte++;
			
			// spielfeld bewegen
			led_shiftLeft();
			for(i = 0; i < 5; i++)
			{
				led_data_buffer[4][i].r = 0;
				led_data_buffer[4][i].g = 0;
				led_data_buffer[4][i].b = 0;
			}
		
			// neue teile spawnen
			for(i = 0; i < d->level; i++)
			{
				if(rand() % 100 > 80)
				{
					tx = rand() % 5;
					led_data_buffer[4][tx].r = LED_COL_MAX;
					led_data_buffer[4][tx].g = 0;
					led_data_buffer[4][tx].b = 0;
				}
			}
			
			if(rand() % 100 > 80)
			{
				tx = rand() % 5;
				led_data_buffer[4][tx].r = LED_COL_MAX;
				led_data_buffer[4][tx].g = LED_COL_MAX;
				led_data_buffer[4][tx].b = 0;				
			}
		
			d->del = d->del_max;
		}
		else
		{
			d->del--;
		}
	
	}
	
	led_copy_buff();
	
	// spieler faden und einblenden
	switch(d->spieler.col_fade)
	{
	case 0:
		if(d->spieler.col.g < LED_COL_MAX)
			d->spieler.col.g++;
		else
			d->spieler.col_fade = 1;	
		break;
		
	case 1:
		if(d->spieler.col.r > 0)
			d->spieler.col.r--;
		else
			d->spieler.col_fade = 2;
			break;
			
	case 2:
		if(d->spieler.col.b < LED_COL_MAX)
			d->spieler.col.b++;
		else
			d->spieler.col_fade = 3;
		break;
	
	case 3:
		if(d->spieler.col.g > 0)
			d->spieler.col.g--;
		else
			d->spieler.col_fade = 4;
		break;
		
	case 4:
		if(d->spieler.col.r < LED_COL_MAX)
			d->spieler.col.r++;
		else
			d->spieler.col_fade = 5;
		break;
	
	case 5:
		if(d->spieler.col.b > 0)
			d->spieler.col.b--;
		else
			d->spieler.col_fade = 0;
		break;
	}
	
	led_data[d->spieler.x][d->spieler.y].r = d->spieler.col.r;
	led_data[d->spieler.x][d->spieler.y].g = d->spieler.col.g;
	led_data[d->spieler.x][d->spieler.y].b = d->spieler.col.b;
	
	// kollisionsabfrage
	if(d->state == STATE_RUN)
	{
		if(led_data_buffer[d->spieler.x][d->spieler.y].r == LED_COL_MAX && led_data_buffer[d->spieler.x][d->spieler.y].g == LED_COL_MAX && led_data_buffer[d->spieler.x][d->spieler.y].b == 0)
		{
			led_data_buffer[d->spieler.x][d->spieler.y].r = 0;
			led_data_buffer[d->spieler.x][d->spieler.y].g = 0;
			led_data_buffer[d->spieler.x][d->spieler.y].b = 0;
			d->punkte += (d->level * 5);
		}
		else if(led_data_buffer[d->spieler.x][d->spieler.y].r == LED_COL_MAX && led_data_buffer[d->spieler.x][d->spieler.y].g == 0 && led_data_buffer[d->spieler.x][d->spieler.y].b == 0)
		{
			audio_stop();
			lcd_print(1, LCD_ATTR_CENTER, "GAME OVER");
			lcd_print(5, LCD_ATTR_CENTER, "SPIEL VORBEI");
			led_setRGB(LED_COL_MAX, 0, 0);
			ps2_set_vibration(255, 1, 5);
			if(app_highscore_isHS(highscore_spiel1, d->punkte))
			{
				d->state = STATE_HS;
				app_highscore_step_init();
			}
			else
			{
				d->state = STATE_END;
			}
		}
	
		lcd_print(3, 0, "Punkte: %lu", d->punkte);		
	}
}