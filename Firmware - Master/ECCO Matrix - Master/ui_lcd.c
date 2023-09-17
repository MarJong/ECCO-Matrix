#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "master.h"
#include "ps2controller.h"
#include "lcd.h"
#include "ui_lcd.h"
#include "audio.h"
#include "relais.h"
#include "animation.h"
#include "app_base.h"

#define MENU_STATE_MENU		1
#define MENU_STATE_FUNC		2

//
void uil_startSubMenu(void);

void uil_startEccoInfo(void);
void uil_inputEccoInfo(void);

void uil_inputRelaisDebug(void);
void uil_drawRelaisDebug(void);
void uil_startRelaisDebug(void);

void uil_drawMenu(void);

menu_entry_t menu_main_e[] =	{
								{"Wer ist der ECCO?", uil_startEccoInfo, uil_inputEccoInfo, NULL},
								{"Animationen", anim_menuStart, anim_menuInput, NULL},
								{"Apps", app_menuStart, app_menuInput, NULL},
								{"Jukebox", audio_menuStart, audio_menuInput, NULL},
//								{"Relaisdebug", uil_startRelaisDebug, uil_inputRelaisDebug, NULL}
								};
menu_t menu_main = {"Hauptmen\x81", 0, 4, menu_main_e};

// (unter)menüs
#define SUB_MENU_MAX		3
menu_t *menu_tracker[SUB_MENU_MAX];
uint8_t menu_sub_pos;

uint8_t menu_state = 0;

/*
	spezifische menüs
*/
void uil_inputEccoInfo(void)
{
	if(ps2_data.btn2_down & PS2_CIRCLE)
	{
		uil_endFunc();
	}
}

void uil_startEccoInfo(void)
{
	menu_state = MENU_STATE_FUNC;
	
	lcd_clr();
				   //http://studiy.tu-
				   //  cottbus.de/ecco
				   //elektronik@
				   //  lists.tu-cottbus.de
	lcd_print(0, LCD_ATTR_CENTER, "Elektronikclub");
	lcd_print(1, LCD_ATTR_CENTER, "Cottbus");
	lcd_print(2, LCD_ATTR_CENTER, "LG10 Raum 239");
	lcd_print(3, LCD_ATTR_CENTER, "http://studiy.tu-");
	lcd_print(4, LCD_ATTR_CENTER, "cottbus.de/ecco");
	lcd_print(5, LCD_ATTR_CENTER, "elektronik@");
	lcd_print(6, LCD_ATTR_CENTER, "lists.tu-cottbus.de");
	lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xE8 zur\x81""ck");
}

// relais debug
uint8_t relais_pos;
void uil_drawRelaisDebug(void)
{
	uint8_t i, fl;
	
	for(i = 0; i < 8; i++)
	{
		fl = 0;
		
		if(i == relais_pos)
			fl = LCD_ATTR_INVERT_T;
			
		if(relais_isActive(i))
		{
			lcd_print(i, fl, "Relais %1d Aktiv", i);
		}
		else
		{
			lcd_print(i, fl, "Relais %1d", i);
		}
	}
}

void uil_startRelaisDebug(void)
{
	relais_pos = 0;
	
	uil_drawRelaisDebug();
	menu_state = MENU_STATE_FUNC;
}

void uil_inputRelaisDebug(void)
{
	uint8_t change = 0;
	
	if(ps2_data.btn1_down & PS2_UP)		// hoch
	{
		if(relais_pos > 0)
		{
			relais_pos--;
			change = 1;
		}
	}
	if(ps2_data.btn1_down & PS2_DOWN)		// runter
	{
		if(relais_pos < 7)
		{
			relais_pos++;
			change = 1;
		}
	}
	
	if(ps2_data.btn2_down & PS2_CROSS)
	{
		if(relais_isActive(relais_pos))
		{
			relais_deactivate(relais_pos);
		}
		else
		{
			relais_activate(relais_pos, 300);
		}
		change = 1;
	}
	
	if(ps2_data.btn2_down & PS2_CIRCLE)
	{
		uil_endFunc();
	}
	
	if(change)
	{
		uil_drawRelaisDebug();
	}
}

/*
	basiskram
*/
// menüstruktur verwalten
void uil_drawMenu(void)
{
	uint8_t start, i;
	
	lcd_clr();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, menu_tracker[menu_sub_pos]->name);
	if(menu_sub_pos > 0)
		lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT,  "\xE7 OK \xEF Ende \xE8 zur\x81""ck");
	else
		lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT,  "\xE7 OK \xEF Ende");
	
	start = (menu_tracker[menu_sub_pos]->pos / 6) * 6;
	
	for(i = 0; i < 6 && start + i < menu_tracker[menu_sub_pos]->max; i++)
	{
		if(start + i == menu_tracker[menu_sub_pos]->pos)
		{
			lcd_print(1 + i, LCD_ATTR_INVERT_T, menu_tracker[menu_sub_pos]->item[start + i]. name);
		}
		else
		{
			lcd_print(1 + i, 0, menu_tracker[menu_sub_pos]->item[start + i].name);
		}
	}
}

void uil_inputMenu(void)
{
	uint8_t change = 0;
	
	if(ps2_data.btn1_down & PS2_UP)		// hoch
	{
		if(menu_tracker[menu_sub_pos]->pos > 0)
		{
			menu_tracker[menu_sub_pos]->pos--;
			change = 1;
		}			
	}
	if(ps2_data.btn1_down & PS2_DOWN)		// runter
	{
		if(menu_tracker[menu_sub_pos]->pos < menu_tracker[menu_sub_pos]->max - 1)
		{
			menu_tracker[menu_sub_pos]->pos++;
			change = 1;
		}
	}
	if(ps2_data.btn2_down & PS2_CROSS)		// bestätigen/auswählen
	{
		if(menu_tracker[menu_sub_pos]->item[menu_tracker[menu_sub_pos]->pos].start)
			menu_tracker[menu_sub_pos]->item[menu_tracker[menu_sub_pos]->pos].start();
	}
	if(ps2_data.btn2_down & PS2_CIRCLE)		// abbrechen/zurück
	{
		if(menu_sub_pos > 0)
		{
			menu_sub_pos--;
			change = 1;
		}			
	}
	
	if(change)
	{
		uil_drawMenu();
	}
}

void uil_startSubMenu(void)
{
	menu_state = MENU_STATE_MENU;
	menu_tracker[menu_sub_pos + 1] = (menu_t*)menu_tracker[menu_sub_pos]->item[menu_tracker[menu_sub_pos]->pos].data;
	menu_sub_pos++;
	
	uil_drawMenu();
}


// starten/init
void uil_start(void)
{
	menu_sub_pos = 0;
	menu_tracker[0] = &menu_main;
	menu_tracker[0]->pos = 0;
	menu_state = MENU_STATE_MENU;
	
	uil_drawMenu();
}

// eingabe auswerten
void uil_input(void)
{
	switch(menu_state)
	{
	case MENU_STATE_MENU:
		uil_inputMenu();
		break;
		
	case MENU_STATE_FUNC:
		if(menu_tracker[menu_sub_pos]->item[menu_tracker[menu_sub_pos]->pos].input)
			menu_tracker[menu_sub_pos]->item[menu_tracker[menu_sub_pos]->pos].input();
		break;
		
	default:
		uil_start();
		break;
	}
}

// func beenden und wieder in menü
void uil_endFunc(void)
{
	menu_state = MENU_STATE_MENU;
	uil_drawMenu();
}

void uil_startFunc(void)
{
	menu_state = MENU_STATE_FUNC;
}

