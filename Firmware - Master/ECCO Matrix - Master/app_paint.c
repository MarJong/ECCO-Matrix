#include <avr/io.h>

#include "app_base.h"
#include "lcd.h"
#include "ps2controller.h"
#include "led.h"

typedef struct {
	uint8_t x, y;
} app_paint_t;

void app_paint_init(void)
{
	app_paint_t *d = (app_paint_t*)app_data;
	
	d->x = 0;
	d->y = 0;
	
	lcd_clear();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Paint");
	lcd_print(2, 0, "\xEA Cursor bewegen");
	lcd_print(3, 0, "Mit Analog-Sticks");
	lcd_print(4, 0, "Farbe ausw\x84hlen");
	lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE8 Ende");
	
	led_setRGB(0, 0, 0);
}

void app_paint_input(void)
{
	app_paint_t *d = (app_paint_t*)app_data;
	
	// cursor bewegen
	if(ps2_data.btn1_down & PS2_UP)
	{
		d->y--;
		if(d->y > 4)
			d->y = 4;
	}
	if(ps2_data.btn1_down & PS2_DOWN)
	{
		d->y++;
		if(d->y > 4)
			d->y = 0;
	}
	
	if(ps2_data.btn1_down & PS2_RIGHT)
	{
		d->x++;
		if(d->x > 4)
			d->x = 0;
	}
	if(ps2_data.btn1_down & PS2_LEFT)
	{
		d->x--;
		if(d->x > 4)
			d->x = 4;
	}
	
	// sonstiges
	if(ps2_data.btn1_down & PS2_START || ps2_data.btn2_down & PS2_CIRCLE)
	{
		app_end();
	}
	
	// farben
	if(ps2_data.lx < PS2_ANALOG_MID)
	{
		if(led_data_buffer[d->x][d->y].r > 0)
		{
			led_data_buffer[d->x][d->y].r--;
		}			
	}
	else if(ps2_data.lx > PS2_ANALOG_MID)
	{
		if(led_data_buffer[d->x][d->y].r < LED_COL_MAX)
		{
			led_data_buffer[d->x][d->y].r++;
		}			
	}

	if(ps2_data.ly < PS2_ANALOG_MID)
	{
		if(led_data_buffer[d->x][d->y].g > 0)
		{
			led_data_buffer[d->x][d->y].g--;
		}
	}
	else if(ps2_data.ly > PS2_ANALOG_MID)
	{
		if(led_data_buffer[d->x][d->y].g < LED_COL_MAX)
		{
			led_data_buffer[d->x][d->y].g++;
		}
	}
	
	if(ps2_data.ry < PS2_ANALOG_MID)
	{
		if(led_data_buffer[d->x][d->y].b > 0)
		{
			led_data_buffer[d->x][d->y].b--;
		}
	}
	else if(ps2_data.ry > PS2_ANALOG_MID)
	{
		if(led_data_buffer[d->x][d->y].b < LED_COL_MAX)
		{
			led_data_buffer[d->x][d->y].b++;
		}
	}
}

void app_paint_draw(void)
{
	app_paint_t *d = (app_paint_t*)app_data;
	led_copy_buff();
	if(ps2_data.lx == PS2_ANALOG_MID && ps2_data.ly == PS2_ANALOG_MID && ps2_data.ry == PS2_ANALOG_MID && ps2_data.rx == PS2_ANALOG_MID)
	{
		led_data[d->x][d->y].r = LED_COL_MAX;
		led_data[d->x][d->y].g = LED_COL_MAX;
		led_data[d->x][d->y].b = LED_COL_MAX;		
	}
}