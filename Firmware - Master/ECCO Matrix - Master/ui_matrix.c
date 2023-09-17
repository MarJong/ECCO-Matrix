/*
	matrix/schaukasten kontrolle
	
	LCD Layout
	0		TITEL
	1 blah1234
	2 mehr text
	3 blub
	4 foo
	5 zuviel text
	6 zzz
	7		STATUS
	
	status
	012345678901234567890
	^D      START      Xv
*/

#include <avr/io.h>

#include "led.h"
#include "ps2controller.h"
#include "ui_matrix.h"
#include "lcd.h"
#include "ui_matrix_texte.h"
#include "master.h"
#include "relais.h"

#define coord_pos(x, y)		(x + (y * 5))

uint8_t cur_x, cur_y;
led_data_t cur_col;

// fluoreszens special
uint8_t flu_mode;
uint32_t flu_tick;
uint8_t flu_blue;
uint8_t flu_relais;

#define FLU_DEL_RELAIS		3000UL
#define FLU_DEL_STEP		50
#define FLU_DEL_COLOR		2000UL

// interne textausgabe
void uim_print(uint8_t pos);
void uim_printText(uint8_t pos, uint8_t offset);

// fluoreszens special func
void flu_init(uint8_t rel)
{
	flu_mode = 0;
	flu_tick = timestamp + FLU_DEL_RELAIS;
	flu_blue = 0;
	flu_relais = rel;
	relais_activate(flu_relais, 10);
}

void flu_step(void)
{
	uint8_t r = 0, g = 0;
	
	switch(flu_mode)
	{
		case 0:		// relais an
			if(timestamp >= flu_tick)
			{
				flu_mode = 1;
//				flu_tick = timestamp + FLU_DEL_STEP;
				relais_deactivate(flu_relais);
			}
			break;
			
		case 1:		// blau an
//			if(timestamp >= flu_tick)
//			{
				if(flu_blue < LED_COL_MAX)
				{
					flu_blue++;
//					flu_tick = timestamp + FLU_DEL_STEP;
				}
				else
				{
					flu_mode = 2;
					flu_tick = timestamp + FLU_DEL_COLOR;
				}
//			}
			break;
			
		case 2:
			if(timestamp >= flu_tick)
			{
				flu_tick = timestamp + FLU_DEL_COLOR;
				flu_mode = 3;
				flu_blue = 0;
			}
			break;
			
		case 3:			// grün
			if(timestamp >= flu_tick)
			{
				flu_tick = timestamp + FLU_DEL_COLOR;
				flu_mode = 4;
				flu_blue = LED_COL_MAX;
			}
			g = LED_COL_MAX;
			break;
			
		case 4:			// blau
			if(timestamp >= flu_tick)
			{
				flu_tick = timestamp + FLU_DEL_COLOR;
				flu_mode = 5;
				flu_blue = 0;
			}
			break;
			
		case 5:			// rot
			if(timestamp >= flu_tick)
			{
				flu_tick = timestamp + FLU_DEL_COLOR;
				flu_mode = 6;
				flu_blue = LED_COL_MAX;
			}
			r = LED_COL_MAX;
			break;
			
		case 6:
			if(timestamp >= flu_tick)
			{
				flu_tick = timestamp + FLU_DEL_COLOR;
				flu_mode = 7;
			}
			break;
			
		case 7:		// blau aus
//			if(timestamp >= flu_tick)
//			{
				if(flu_blue > 0)
				{
					flu_blue--;
//					flu_tick = timestamp + FLU_DEL_STEP;
				}
				else
				{
					flu_mode = 0;
					flu_tick = timestamp + FLU_DEL_RELAIS;
					relais_activate(flu_relais, 10);
				}
//			}
			break;
	}
	
	led_data[cur_x][cur_y].r = r;
	led_data[cur_x][cur_y].g = g;
	led_data[cur_x][cur_y].b = flu_blue;
}



//

void uim_start(void)
{
	cur_x = 2;
	cur_y = 3;
	cur_col.r = LED_COL_MAX;
	cur_col.g = LED_COL_MAX;
	cur_col.b = LED_COL_MAX;
	
	uim_print(coord_pos(cur_x, cur_y));
}

void uim_overlay(void)
{
	uint8_t temp;
	
	led_data[cur_x][cur_y].r = cur_col.r;
	led_data[cur_x][cur_y].g = cur_col.g;
	led_data[cur_x][cur_y].b = cur_col.b;	
	
	temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
/*
	if(temp & FLAG_CANACTIVATE)
	{
		if(relais_isActive(UIM_GETRELAIS(temp)) && (temp & FLAG_DARK))
		{
			led_data[cur_x][cur_y].r = 0;
			led_data[cur_x][cur_y].g = 0;
			led_data[cur_x][cur_y].b = 0;	
		}
	}
*/
	if(temp & FLAG_FLUOR_SPECIAL)
	{
		flu_step();
	}
	else if(temp & FLAG_DARK)
	{
		led_data[cur_x][cur_y].r = 0;
		led_data[cur_x][cur_y].g = 0;
		led_data[cur_x][cur_y].b = 0;
	}
}

void uim_print(uint8_t pos)
{
	char temp[22];
	uint8_t i;

	for(i = 0; i < 21; i++)
	{
		temp[i] = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].title[i]);
		if(!temp[i])
			break;
	}
	temp[i] = 0;
	lcd_print(0, LCD_ATTR_INVERT | LCD_ATTR_CENTER, temp);
	
//	temp[0] = 0;
/*
	i = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
	if(i & FLAG_CANACTIVATE)
	{
		lcd_print(7, LCD_ATTR_INVERT | LCD_ATTR_CENTER, "\xE7 Start \xE8 Zur\x81""ck");
	}
	else
	{
		lcd_print(7, LCD_ATTR_INVERT | LCD_ATTR_CENTER, "\xEF Men\x81 \xE8 Zur\x81""ck");
	}
	*/
	lcd_print(7, LCD_ATTR_INVERT | LCD_ATTR_CENTER, "\xEF Men\x81 \xE8 Zur\x81""ck");
	
	uim_printText(pos, 0);
}

void uim_printText(uint8_t pos, uint8_t offset)
{
	char temp[22];
	uint8_t start, z = 0, i;
	
	start = offset * 6;
	
	while(z < 6)
	{
		for(i = 0; i < 21; i++)
		{
			temp[i] = pgm_read_byte(&projekt_texte[pos][start + z][i]);
			if(!temp[i])
				break;
		}
		temp[i] = 0;
			
		lcd_print(z + 1, 0, temp);
		z++;
	}
}

void uim_input(void)
{
	uint8_t temp;
	
	// cursor
	if(ps2_data.btn1_down & PS2_UP)
	{
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_CANACTIVATE)
		{
			relais_deactivate(UIM_GETRELAIS(temp));
		}
		
		if(cur_y > 0)
			cur_y--;
		else
			cur_y = 4;
		
		uim_print(coord_pos(cur_x, cur_y));
		
		// relais starten
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_FLUOR_SPECIAL)
		{
			flu_init(UIM_GETRELAIS(temp));
		}
		else if(temp & FLAG_CANACTIVATE)
		{
			relais_activate(UIM_GETRELAIS(temp), STATE_TIMEOUT_LEN / 1000);
		}
	}
	if(ps2_data.btn1_down & PS2_DOWN)
	{
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_CANACTIVATE)
		{
			relais_deactivate(UIM_GETRELAIS(temp));
		}
		
		if(cur_y < 4)
			cur_y++;
		else
			cur_y = 0;
			
		uim_print(coord_pos(cur_x, cur_y));
		
		// relais starten
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_FLUOR_SPECIAL)
		{
			flu_init(UIM_GETRELAIS(temp));
		}
		else if(temp & FLAG_CANACTIVATE)
		{
			relais_activate(UIM_GETRELAIS(temp), STATE_TIMEOUT_LEN / 1000);
		}
	}
	if(ps2_data.btn1_down & PS2_LEFT)
	{
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_CANACTIVATE)
		{
			relais_deactivate(UIM_GETRELAIS(temp));
		}
		
		if(cur_x > 0)
			cur_x--;
		else
			cur_x = 4;
			
		uim_print(coord_pos(cur_x, cur_y));
		
		// relais starten
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_FLUOR_SPECIAL)
		{
			flu_init(UIM_GETRELAIS(temp));
		}
		else if(temp & FLAG_CANACTIVATE)
		{
			relais_activate(UIM_GETRELAIS(temp), STATE_TIMEOUT_LEN / 1000);
		}
	}
	if(ps2_data.btn1_down & PS2_RIGHT)
	{
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_CANACTIVATE)
		{
			relais_deactivate(UIM_GETRELAIS(temp));
		}

		if(cur_x < 4)
			cur_x++;
		else
			cur_x = 0;
			
		uim_print(coord_pos(cur_x, cur_y));
		
		// relais starten
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_FLUOR_SPECIAL)
		{
			flu_init(UIM_GETRELAIS(temp));
		}
		else if(temp & FLAG_CANACTIVATE)
		{
			relais_activate(UIM_GETRELAIS(temp), STATE_TIMEOUT_LEN / 1000);
		}
	}
	
	/*
	if(ps2_data.btn2_down & PS2_CROSS)
	{
		// relais starten
		temp = pgm_read_byte(&projekt_info[coord_pos(cur_x, cur_y)].flags);
		if(temp & FLAG_CANACTIVATE)
		{
			if(relais_isActive(UIM_GETRELAIS(temp)))
				relais_deactivate(UIM_GETRELAIS(temp));
			else
				relais_activate(UIM_GETRELAIS(temp), STATE_TIMEOUT_LEN / 1000);
		}
	}
	*/
}
