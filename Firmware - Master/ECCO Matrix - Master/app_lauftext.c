#include <avr/io.h>
#include <stdlib.h>

#include "ps2controller.h"
#include "app_base.h"
#include "lcd.h"
#include "led.h"

#define MAX_TEXT_LEN		20
#define BUTTON_REPEAT_COUNT	5

typedef struct {
	char text[MAX_TEXT_LEN + 1];
	uint8_t pos, btn_cnt, run;
} app_text_t;

void app_lauftext_init(void)
{
	app_text_t *d = (app_text_t*)app_data;
	
	d->text[0] = 'A';
	d->text[1] = '\0';
	d->pos = 0;
	d->btn_cnt = 0;
	d->run = 1;
	
	led_setRGB(0, 0, 0);
	
	led_font_start("LAUFTEXT", (15 + (rand() % (LED_COL_MAX - 15))), (15 + (rand() % (LED_COL_MAX - 15))), (15 + (rand() % (LED_COL_MAX - 15))));
	
	lcd_clear();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Lauftext");
	lcd_print(1, 0, "\xEA ausw\x84hlen");
	lcd_print(2, 0, "\xE7 OK \xE8 l\x94schen");
	lcd_print(3, 0, "\xEF Text starten");
	lcd_print(4, LCD_ATTR_CENTER, "Eingabe:");
	lcd_print(7, LCD_ATTR_INVERT | LCD_ATTR_CENTER, "\xE6 Beenden");
}

void app_lauftext_input(void)
{
	app_text_t *d = (app_text_t*)app_data;
	
	if(ps2_data.btn2_down & PS2_SQUARE)
	{
		app_end();
	}
	
	if(ps2_data.btn1_down & PS2_UP)
	{
		d->btn_cnt = BUTTON_REPEAT_COUNT;
		d->text[d->pos] += 5;
		if(d->text[d->pos] > 'Z')
			d->text[d->pos] = ' ';
	}
	else if(!(ps2_data.btn1 & PS2_UP))
	{
		if(!d->btn_cnt--)
		{
			d->btn_cnt = BUTTON_REPEAT_COUNT;
			d->text[d->pos] += 5;
			if(d->text[d->pos] > 'Z')
				d->text[d->pos] = ' ';
		}
	}
	
	if(ps2_data.btn1_down & PS2_DOWN)
	{
		d->btn_cnt = BUTTON_REPEAT_COUNT;
		d->text[d->pos] -= 5;
		if(d->text[d->pos] < ' ')
			d->text[d->pos] = 'Z';
	}
	else if(!(ps2_data.btn1 & PS2_DOWN))
	{
		if(!d->btn_cnt--)
		{
			d->btn_cnt = BUTTON_REPEAT_COUNT;
			d->text[d->pos] -= 5;
			if(d->text[d->pos] < ' ')
				d->text[d->pos] = 'Z';
		}
	}
	
	if(ps2_data.btn1_down & PS2_RIGHT)
	{
		d->btn_cnt = BUTTON_REPEAT_COUNT;
		d->text[d->pos]++;
		if(d->text[d->pos] > 'Z')
			d->text[d->pos] = ' ';
	}
	else if(!(ps2_data.btn1 & PS2_RIGHT))
	{
		if(!d->btn_cnt--)
		{
			d->btn_cnt = BUTTON_REPEAT_COUNT;
			d->text[d->pos]++;
			if(d->text[d->pos] > 'Z')
				d->text[d->pos] = ' ';
		}
	}
	
	if(ps2_data.btn1_down & PS2_LEFT)
	{
		d->btn_cnt = BUTTON_REPEAT_COUNT;
		d->text[d->pos]--;
		if(d->text[d->pos] < ' ')
			d->text[d->pos] = 'Z';
	}
	else if(!(ps2_data.btn1 & PS2_LEFT))
	{
		if(!d->btn_cnt--)
		{
			d->btn_cnt = BUTTON_REPEAT_COUNT;
			d->text[d->pos]--;
			if(d->text[d->pos] < ' ')
				d->text[d->pos] = 'Z';
		}
	}
	
	if(ps2_data.btn2_down & PS2_CROSS)
	{
		if(d->pos < MAX_TEXT_LEN - 1)
		{
			d->pos++;
			d->text[d->pos] = 'A';
			d->text[d->pos + 1] = '\0';
		}
	}
	else if(ps2_data.btn2_down & PS2_CIRCLE)
	{
		if(d->pos > 0)
		{
			d->text[d->pos] = '\0';
			d->pos--;
		}
	}
	else if(ps2_data.btn1_down & PS2_START)
	{
		d->run = 1;
		led_font_start(d->text, (15 + (rand() % (LED_COL_MAX - 15))), (15 + (rand() % (LED_COL_MAX - 15))), (15 + (rand() % (LED_COL_MAX - 15))));
		d->text[0] = 'A';
		d->text[1] = '\0';
		d->pos = 0;
	}
	
	lcd_print(5, LCD_ATTR_CENTER, d->text);
}

void app_lauftext_draw(void)
{
	app_text_t *d = (app_text_t*)app_data;
	
	if(d->run)
	{
		if(!led_font_step())
		{
			d->run = 0;
		}
		led_copy_buff();
		led_font_merge();
	}
}