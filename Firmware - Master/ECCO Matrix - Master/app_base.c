#include <avr/io.h>
#include <stdio.h>

#include "master.h"
#include "app_base.h"
#include "ui_lcd.h"
#include "lcd.h"
#include "ps2controller.h"

#include "app_paint.h"
#include "app_spiel1.h"
#include "app_snake.h"
#include "app_lauftext.h"

extern volatile uint32_t timestamp;
extern uint32_t state_timeout;
extern uint8_t main_state;

uint8_t app_data[APP_DATA_SIZE];

#define APP_NUM		4
app_list_t app_list[APP_NUM] = {
	{"Paint", app_paint_init, app_paint_input, app_paint_draw},
	{"Lauftext", app_lauftext_init, app_lauftext_input, app_lauftext_draw},
	{"Spiel 1", app_spiel1_init, app_spiel1_input, app_spiel1_draw},
	{"Snake", app_snake_init, app_snake_input, app_snake_draw}
	};

uint8_t app_cur = 0;

// app num starten
void app_start(uint8_t num)
{
	if(num >= APP_NUM)
		return;
	
	app_cur = num;
	main_state = STATE_APP;
	
	if(app_list[app_cur].func_init)
		app_list[app_cur].func_init();
}

// app beenden
void app_end(void)
{
	state_timeout = timestamp;
}

// app eingabe event
void app_input(void)
{
	if(app_list[app_cur].func_input)
		app_list[app_cur].func_input();
}

// app ausgabe
void app_draw(void)
{
	if(app_list[app_cur].func_draw)
		app_list[app_cur].func_draw();	
}

// menü interface
uint8_t app_MenuPos;
void app_menuDraw(void)
{
	uint8_t start, i = 0;
	
	lcd_clr();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Apps");
	lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE7 Start \xE8 zur\x81""ck");
	
	start = (app_MenuPos / 6) * 6;
	
	for(i = 0; i < 6 && start + i < APP_NUM; i++)
	{
		if(start + i == app_MenuPos)
		{
			lcd_print(1 + i, LCD_ATTR_INVERT_T, app_list[start + i].name);
		}
		else
		{
			lcd_print(1 + i, 0, app_list[start + i].name);
		}
	}
}

void app_menuStart(void)
{
	uil_startFunc();
	app_MenuPos = 0;
	
	app_menuDraw();
}

void app_menuInput(void)
{
	if(ps2_data.btn1_down & PS2_UP)		// hoch
	{
		if(app_MenuPos > 0)
		{
			app_MenuPos--;
			app_menuDraw();
		}
	}
	else if(ps2_data.btn1_down & PS2_DOWN)		// runter
	{
		if(app_MenuPos < APP_NUM - 1)
		{
			app_MenuPos++;
			app_menuDraw();
		}
	}
	
	if(ps2_data.btn2_down & PS2_CROSS)		// bestätigen/auswählen
	{
		app_start(app_MenuPos);
	}
	
	if(ps2_data.btn2_down & PS2_CIRCLE)		// abbrechen/zurück
	{
		uil_endFunc();
	}
}