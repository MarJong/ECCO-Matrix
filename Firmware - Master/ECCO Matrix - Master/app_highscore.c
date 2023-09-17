#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>

#include "app_highscore.h"
#include "lcd.h"
#include "ps2controller.h"

app_highscore_t highscore_snake[HIGHSCORE_MAX], highscore_spiel1[HIGHSCORE_MAX];

app_highscore_t EEMEM highscore_snake_ee[HIGHSCORE_MAX], highscore_spiel1_ee[HIGHSCORE_MAX];

// temp. variablen für namen einlesen
#define BUTTON_REPEAT_COUNT 5
char hs_name[HIGHSCORE_NAME_MAX];
uint8_t hs_pos, hs_btn_cnt;

void app_highscore_init(void)
{
	eeprom_read_block(highscore_snake, highscore_snake_ee, sizeof(app_highscore_t) * HIGHSCORE_MAX);
	eeprom_read_block(highscore_spiel1, highscore_spiel1_ee, sizeof(app_highscore_t) * HIGHSCORE_MAX);
}

void inline app_highscore_save_spiel1(void)
{
	eeprom_write_block(highscore_spiel1, highscore_spiel1_ee, sizeof(app_highscore_t) * HIGHSCORE_MAX);
}

void inline app_highscore_save_snake(void)
{
	eeprom_write_block(highscore_snake, highscore_snake_ee, sizeof(app_highscore_t) * HIGHSCORE_MAX);
}

// returns 1 wenn in liste, 0 wenn nicht
uint8_t app_highscore_isHS(app_highscore_t *l, uint32_t p)
{
	if(p > l[HIGHSCORE_MAX - 1].punkte)
		return 1;
	return 0;
}

// ausgabe top 5
void app_highscore_print(app_highscore_t *l)
{
	uint8_t i;
	
	lcd_clear();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "HighScore");
	for(i = 0; i < HIGHSCORE_MAX; i++)
	{
		lcd_print(i + 1, 0, "%-7s %u", l[i].name, l[i].punkte);
	}
	lcd_print(7, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "\xEA Anleitung");
}

// eingabe neuer highscore
void app_highscore_addHS(app_highscore_t *l, uint32_t punkte)
{
	uint8_t pos, i;
	
	// pos für neue punkte suchen
	for(pos = 0; pos < HIGHSCORE_MAX; pos++)
	{
		if(punkte > l[pos].punkte)
			break;
	}
	
	// alles ab pos 1 runter
	for(i = HIGHSCORE_MAX - 1; i > pos; i--)
	{
		strcpy(l[i].name, l[i - 1].name);
		l[i].punkte = l[i - 1].punkte;
	}
	
	l[pos].punkte = punkte;
	strcpy(l[pos].name, hs_name);
}

// init für step
void app_highscore_step_init(void)
{
	hs_pos = 0;
	hs_name[0] = 'A';
	hs_name[1] = '\0';
	hs_btn_cnt = BUTTON_REPEAT_COUNT;
	
	lcd_clear();
	lcd_print(0, LCD_ATTR_CENTER | LCD_ATTR_INVERT, "Neuer Highscore");
	lcd_print(1, 0, "\xEA ausw\x84hlen");
	lcd_print(2, 0, "\xE7 OK \xE8 l\x94schen");
	lcd_print(4, LCD_ATTR_CENTER, "Eingabe:");
	lcd_print(7, LCD_ATTR_INVERT | LCD_ATTR_CENTER, "\xEF Speichern");
}

// step zum namen einlesen return 1 wenn eingabe fertig, sonst 0
uint8_t app_highscore_step(void)
{
	if(ps2_data.btn1_down & PS2_UP)
	{
		hs_btn_cnt = BUTTON_REPEAT_COUNT;
		hs_name[hs_pos] += 5;
		if(hs_name[hs_pos] > 'z')
			hs_name[hs_pos] = ' ';
	}
	else if(!(ps2_data.btn1 & PS2_UP))
	{
		if(!hs_btn_cnt--)
		{
			hs_btn_cnt = BUTTON_REPEAT_COUNT;
			hs_name[hs_pos] += 5;
			if(hs_name[hs_pos] > 'z')
				hs_name[hs_pos] = ' ';
		}
	}
	
	if(ps2_data.btn1_down & PS2_DOWN)
	{
		hs_btn_cnt = BUTTON_REPEAT_COUNT;
		hs_name[hs_pos] -= 5;
		if(hs_name[hs_pos] < ' ')
			hs_name[hs_pos] = 'z';
	}
	else if(!(ps2_data.btn1 & PS2_DOWN))
	{
		if(!hs_btn_cnt--)
		{
			hs_btn_cnt = BUTTON_REPEAT_COUNT;
			hs_name[hs_pos] -= 5;
			if(hs_name[hs_pos] < ' ')
				hs_name[hs_pos] = 'z';
		}
	}
	
	if(ps2_data.btn1_down & PS2_RIGHT)
	{
		hs_btn_cnt = BUTTON_REPEAT_COUNT;
		hs_name[hs_pos]++;
		if(hs_name[hs_pos] > 'z')
			hs_name[hs_pos] = ' ';
	}
	else if(!(ps2_data.btn1 & PS2_RIGHT))
	{
		if(!hs_btn_cnt--)
		{
			hs_btn_cnt = BUTTON_REPEAT_COUNT;
			hs_name[hs_pos]++;
			if(hs_name[hs_pos] > 'z')
				hs_name[hs_pos] = ' ';
		}
	}
	
	if(ps2_data.btn1_down & PS2_LEFT)
	{
		hs_btn_cnt = BUTTON_REPEAT_COUNT;
		hs_name[hs_pos]--;
		if(hs_name[hs_pos] < ' ')
			hs_name[hs_pos] = 'z';
	}
	else if(!(ps2_data.btn1 & PS2_LEFT))
	{
		if(!hs_btn_cnt--)
		{
			hs_btn_cnt = BUTTON_REPEAT_COUNT;
			hs_name[hs_pos]--;
			if(hs_name[hs_pos] < ' ')
				hs_name[hs_pos] = 'z';
		}
	}
	
	if(ps2_data.btn2_down & PS2_CROSS)
	{
		if(hs_pos < HIGHSCORE_NAME_MAX - 1)
		{
			hs_pos++;
			hs_name[hs_pos] = 'A';
			hs_name[hs_pos + 1] = '\0';
		}
	}
	else if(ps2_data.btn2_down & PS2_CIRCLE)
	{
		if(hs_pos > 0)
		{
			hs_name[hs_pos] = '\0';
			hs_pos--;
		}
	}
	else if(ps2_data.btn1_down & PS2_START)
	{
		return 1;
	}
	
	lcd_print(5, LCD_ATTR_CENTER, hs_name);
	
	return 0;
}