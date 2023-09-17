#include <avr/io.h>
#include <stdlib.h>

#include "app_base.h"
#include "lcd.h"
#include "ps2controller.h"
#include "led.h"
#include "master.h"
#include "audio.h"
#include "app_highscore.h"

#define MAX_LEN		20
#define ZEIT_BASE	20

#define STATE_START		0
#define STATE_SPIEL		1
#define STATE_ENDE		2
#define STATE_HS		3

typedef enum {RECHTS = 0, RUNTER, LINKS, HOCH} richtung_t;

typedef struct {
	uint8_t state;
	struct {
		uint8_t x, y;
		richtung_t dir;
		uint8_t len, mlen, head, tail;
		struct {
			uint8_t x, y;
		} body[MAX_LEN];
	} spieler;
	
	uint8_t del, del_max;
	uint8_t gelb_num;
	uint16_t punkte;
	uint32_t startzeit;
	uint8_t lcd_display;		// 0 = einleitung, 1 = highscore
} app_snake_t;

void app_snake_ende(void)
{
	audio_stop();
	app_end();
}

void app_snake_draw_lcd(void)
{
	lcd_clear();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Snake");
	lcd_print(1, 0, "\xEA Bewegen");
	lcd_print(2, 0, "Ziel:");
	lcd_print(3, 0, "Nicht mit sich sel-");
	lcd_print(4, 0, "ber kollidieren");
	lcd_print(5, 0, "Gelb einsammeln");
	lcd_print(6, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xEA HighScore");
	lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xEF Start \xE6 Ende");
}

void app_snake_init(void)
{
	app_snake_t *d = (app_snake_t*)app_data;
	
	d->state = STATE_START;
	d->spieler.x = 2;
	d->spieler.y = 2;
	d->spieler.len = 1;
	d->spieler.body[0].x = 2;
	d->spieler.body[0].y = 3;
	d->spieler.mlen = 2;
	d->spieler.dir = HOCH;
	d->spieler.head = 0;
	d->spieler.tail = 0;
	d->gelb_num = 0;
	
	d->del = 0;
	d->del_max = ZEIT_BASE;
	d->punkte= 0;
	d->startzeit = 0;
	
	app_snake_draw_lcd();
	d->lcd_display = 0;
	
	led_setRGB(0, 0, 0);
	led_copy_buff();
	led_data[d->spieler.x][d->spieler.y].r = 0;
	led_data[d->spieler.x][d->spieler.y].g = LED_COL_MAX;
	led_data[d->spieler.x][d->spieler.y].b = 0;
}

void app_snake_input(void)
{
	app_snake_t *d = (app_snake_t*)app_data;
	
	if(ps2_data.btn2_down & PS2_SQUARE)
	{
		app_snake_ende();
		return;
	}
	
	switch(d->state)
	{
	case STATE_START:
		if(ps2_data.btn1_down & (PS2_UP | PS2_DOWN | PS2_LEFT | PS2_RIGHT))
		{
			if(d->lcd_display == 0)
			{
				d->lcd_display = 1;
				app_highscore_print(highscore_snake);
			}
			else
			{
				d->lcd_display = 0;
				app_snake_draw_lcd();
			}
		}
		
		if(ps2_data.btn1_down & PS2_START)
		{
			lcd_clear();
			lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Snake");
			lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE6 Ende");
			d->state = STATE_SPIEL;
			// audio_play(3, 1, 0);
			audio_play_random();
			d->startzeit = timestamp;
		}
		break;
		
	case STATE_SPIEL:
		if(ps2_data.btn1_down & PS2_UP)
		{
			if(d->spieler.dir != RUNTER)
			{
				d->spieler.dir = HOCH;
			}				
		}
		else if(ps2_data.btn1_down & PS2_DOWN)
		{
			if(d->spieler.dir != HOCH)
			{
				d->spieler.dir = RUNTER;
			}
		}
		else if(ps2_data.btn1_down & PS2_LEFT)
		{
			if(d->spieler.dir != RECHTS)
			{
				d->spieler.dir = LINKS;
			}
		}
		else if(ps2_data.btn1_down & PS2_RIGHT)
		{
			if(d->spieler.dir != LINKS)
			{
				d->spieler.dir = RECHTS;
			}
		}
		break;
		
	case STATE_ENDE:
		if((ps2_data.btn1_down & 0x0F) != 0 || ps2_data.btn2_down != 0)
		{
			app_snake_init();
		}
		break;
		
	case STATE_HS:
		if(app_highscore_step())
		{
			app_highscore_addHS(highscore_snake, d->punkte);
			app_highscore_save_snake();
			app_snake_init();
		}
		break;
	}
}

void app_snake_draw(void)
{
	app_snake_t *d = (app_snake_t*)app_data;
	uint32_t zdiff = (timestamp - d->startzeit);
	uint8_t i, pos;
	
	if(d->state == STATE_SPIEL)
	{
		lcd_print(4, 0, "Zeit: %.1f", (float)((float)zdiff / 1000.0f));
		if(d->del)
		{
			d->del--;
		}
		else		// frame berechnen
		{
			
			// bewegen
			switch(d->spieler.dir)
			{
			case HOCH:
				d->spieler.y--;
				if(d->spieler.y > 4)
					d->spieler.y = 4;
				break;	
			
			case RUNTER:
				d->spieler.y++;
				if(d->spieler.y > 4)
					d->spieler.y = 0;
				break;
				
			case LINKS:
				d->spieler.x--;
				if(d->spieler.x > 4)
					d->spieler.x = 4;
				break;
				
			case RECHTS:
				d->spieler.x++;
				if(d->spieler.x > 4)
					d->spieler.x = 0;
				break;
			}
			
			// kollision
			if(led_data[d->spieler.x][d->spieler.y].r == 0 && led_data[d->spieler.x][d->spieler.y].g > 0 && led_data[d->spieler.x][d->spieler.y].b > 0)
			{
				// schlange
				for(i = 0; i < 5; i++)
				{
					for(pos = 0; pos < 5; pos++)
					{
						if(led_data[i][pos].b == 0 && led_data[i][pos].r == 0)
						{
							led_data[i][pos].r = LED_COL_MAX;
							led_data[i][pos].g = 0;
							led_data[i][pos].b = 0;
						}
					}
				}

				audio_stop();
				lcd_print(1, LCD_ATTR_CENTER, "GAME OVER");
				lcd_print(5, LCD_ATTR_CENTER, "SPIEL VORBEI");
				
				ps2_set_vibration(255, 1, 5);			
	
				if(app_highscore_isHS(highscore_snake, d->punkte))
				{
					d->state = STATE_HS;
					app_highscore_step_init();
				}
				else
				{
					d->state = STATE_ENDE;
				}
	
				return;
			}
			else if(led_data_buffer[d->spieler.x][d->spieler.y].r == LED_COL_MAX && led_data_buffer[d->spieler.x][d->spieler.y].g == LED_COL_MAX)
			{
				// punktding
				d->punkte += 15;
				if(d->gelb_num)
					d->gelb_num--;
				
				led_data_buffer[d->spieler.x][d->spieler.y].r = 0;
				led_data_buffer[d->spieler.x][d->spieler.y].g = 0;
				
				if(d->punkte & 1)
				{
					if(d->spieler.mlen < MAX_LEN)
						d->spieler.mlen++;
					if(d->del_max)
						d->del_max--;
				}
			}
			
			// gelbes spawnen
			if(d->gelb_num == 0 || rand() % 100 > 80)
			{
				i = rand() % 5;
				pos = rand() % 5;
				
				led_data_buffer[i][pos].r = LED_COL_MAX;
				led_data_buffer[i][pos].g = LED_COL_MAX;
				led_data_buffer[i][pos].b = 0;
				
				d->gelb_num++;
			}
			
			// ausgabe
			led_copy_buff();
			led_data[d->spieler.x][d->spieler.y].r = 0;
			led_data[d->spieler.x][d->spieler.y].g = LED_COL_MAX;
			led_data[d->spieler.x][d->spieler.y].b = 0;
			
			if(d->spieler.len > 0)
			{
				i = d->spieler.tail;
				pos = 1;
				
				d->spieler.body[d->spieler.head].x = d->spieler.x;
				d->spieler.body[d->spieler.head].y = d->spieler.y;
				
				do {
					led_data[d->spieler.body[i].x][d->spieler.body[i].y].r = 0;
					led_data[d->spieler.body[i].x][d->spieler.body[i].y].g = LED_COL_MAX - pos;
					led_data[d->spieler.body[i].x][d->spieler.body[i].y].b = pos;
					
					pos++;
					i--;
					if(i >= d->spieler.len)
						i = d->spieler.len - 1;
				} while(i != d->spieler.head);
				
				d->spieler.head++;
				if(d->spieler.head >= d->spieler.len)
				{
					if(d->spieler.mlen > d->spieler.len)
					{
						d->spieler.len++;
					}
					else
					{
						d->spieler.head = 0;
					}
				}
				
				d->spieler.tail++;
				if(d->spieler.tail >= d->spieler.len)
				{
					d->spieler.tail = 0;
				}
			}
			else
			{
				if(d->spieler.mlen > d->spieler.len)	// erstes hinzufügen
				{
					d->spieler.head = 0;
					d->spieler.tail = 0;
					
					d->spieler.body[0].x = d->spieler.x;
					d->spieler.body[0].y = d->spieler.y;
					
					d->spieler.len++;
				}
			}
			
			
			lcd_print(3, 0, "Punkte: %d", d->punkte);
			
			d->del = d->del_max;
		}
	}
}