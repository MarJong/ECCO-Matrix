#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "lcd.h"
#include "master.h"
#include "font.h"

uint8_t lcd_flags;		// welche zeile neu gezeichnet werden muss

char lcd_buffer[8][22];				// 8 zeilen, 21 zeichen pro zeile
uint8_t lcd_buffer_attr[8];			// attribute für zeile
uint8_t lcd_output_buffer[128];		// ausgabedaten für 1 zeile

// low level zugriffe
void lcd_write(uint8_t data)
{
	LCD_DATA = data;
	LCD_DELAY_500ns;
	sbi(LCD_PORT, (1 << LCD_E));
	LCD_CLK_DELAY;
	cbi(LCD_PORT, (1 << LCD_E));
	LCD_CLK_DELAY;
}

uint8_t lcd_isBusy_(void)
{
	uint8_t ret;
	
	LCD_DATA_PORT_IN;
	
	LCD_STATUS_MODE;
	sbi(LCD_PORT, (1 << LCD_E));
	LCD_CLK_DELAY;
	ret = LCD_DATA_PIN & (1 << 7);	// busy bit
	cbi(LCD_PORT, (1 << LCD_E));
	LCD_CLK_DELAY;
	
	LCD_DATA_PORT_OUT;
	
	return ret;
}

// CS1 busy?
uint8_t inline lcd_isBusy1(void)
{
	LCD_CS1_ON;
	
	return lcd_isBusy_();
}

// CS2 busy?
uint8_t inline lcd_isBusy2(void)
{
	LCD_CS2_ON;
	
	return lcd_isBusy_();
}

// warten bis beide chips fertig
void inline lcd_waitBusy(void)
{
	while(lcd_isBusy1() || lcd_isBusy2())
		;
}

// init
void lcd_init(void)
{
	uint8_t ready, i;
	
	cbi(LCD_PORT, (1 << LCD_RESET));
	_delay_ms(100);
	sbi(LCD_PORT, (1 << LCD_RESET));
	
	LCD_DATA_PORT_IN;
	
	do 
	{
		ready = 0;
		
		LCD_CS1_ON;
		LCD_STATUS_MODE;
		sbi(LCD_PORT, (1 << LCD_E));
		LCD_CLK_DELAY;
		ready |= LCD_DATA_PIN & (1 << 4);	// reset bit
		cbi(LCD_PORT, (1 << LCD_E));
		LCD_CLK_DELAY;
		
		LCD_CS2_ON;
		LCD_STATUS_MODE;
		sbi(LCD_PORT, (1 << LCD_E));
		LCD_CLK_DELAY;
		ready |= LCD_DATA_PIN & (1 << 4);	// reset bit
		cbi(LCD_PORT, (1 << LCD_E));
		LCD_CLK_DELAY;
	} while(ready);
	
	LCD_DATA_PORT_OUT;
	
	lcd_waitBusy();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_ON);
	_delay_ms(1);
	
	lcd_waitBusy();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_DISP | 0);
	_delay_ms(1);
	
	lcd_waitBusy();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_SETADD | 0);
	_delay_ms(1);
		
	lcd_waitBusy();
	LCD_CS1_CS2_ON;
	LCD_CMD_MODE;
	lcd_write(LCD_CMD_SETPAGE | 0);
	_delay_ms(1);
	
	LCD_CS_OFF;
	
	lcd_flags = 0;
	
	for(i = 0; i < 8; i++)
	{
		lcd_buffer_attr[i] = 0;
		lcd_buffer[i][0] = 0;
	}
}

// alles löschen (nach init)
void lcd_clear(void)
{
	uint16_t i = 0, l = 0;
	
	LCD_CS1_ON;
	
	l = 0;
	for(i = 0; i < 512; i++)
	{
		while(lcd_isBusy1())
			;
		
		if(i % 64 == 0)
		{
			l++;
			LCD_CMD_MODE;
			lcd_write(LCD_CMD_SETPAGE | l);
			while(lcd_isBusy1())
				;
		}
		
		LCD_DATA_W_MODE;
		lcd_write(0);
	}

	LCD_CS2_ON;
		
	l = 0;
	for(i = 0; i < 512; i++)
	{
		while(lcd_isBusy2())
			;

		if(i % 64 == 0)
		{
			l++;
			LCD_CMD_MODE;
			lcd_write(LCD_CMD_SETPAGE | l);
			while(lcd_isBusy2())
				;
		}

		LCD_DATA_W_MODE;
		lcd_write(0);
	}
}

// text ausgeben
void lcd_print(uint8_t zeile, uint8_t attr, char *str, ...)
{
	va_list val;
	
	if(zeile > 7)
		return;
	
	va_start(val, str);
	vsprintf(lcd_buffer[zeile], str, val);
	va_end(val);
	
	lcd_buffer[zeile][21] = 0;
	lcd_buffer_attr[zeile] = attr;
	
	lcd_flags |= (1 << zeile);
}

// löschen für im-programm
void lcd_clr(void)
{
	uint8_t i;
	
	for(i = 0; i < 8; i++)
	{
		lcd_buffer_attr[i] = 0;
		lcd_buffer[i][0] = 0;
	}
	
	lcd_flags = 0xFF;
}

// erstellt ausgabebuffer zu zeile
void lcd_toBuffer(uint8_t zeile)
{
	uint8_t i, j, pos = 0, invert = 0, start = 0;
	
	
	if(lcd_buffer_attr[zeile] & LCD_ATTR_CENTER)
	{
		start = 64 - ((strlen(lcd_buffer[zeile]) * FONT_WIDTH) / 2);
		invert = lcd_buffer_attr[zeile] & LCD_ATTR_INVERT;
		
		while(pos < start)
		{
			if(invert)
				lcd_output_buffer[pos] = 0xFF;
			else
				lcd_output_buffer[pos] = 0;
			pos++;
		}
	}
	
	invert = lcd_buffer_attr[zeile] & (LCD_ATTR_INVERT_T | LCD_ATTR_INVERT);
	for(i = 0; i < 22 && lcd_buffer[zeile][i]; i++)
	{
		for(j = 0; j < FONT_WIDTH; j++)
		{
			if(invert)
				lcd_output_buffer[pos] = ~pgm_read_byte(&font[(uint8_t)lcd_buffer[zeile][i]][j]);
			else
				lcd_output_buffer[pos] = pgm_read_byte(&font[(uint8_t)lcd_buffer[zeile][i]][j]);
			pos++;
		}
	}
	
	invert = lcd_buffer_attr[zeile] & LCD_ATTR_INVERT;
	while(pos < 128)
	{
		if(invert)
			lcd_output_buffer[pos] = 0xFF;
		else
			lcd_output_buffer[pos] = 0;
		pos++;
	}
}

// benötigt ausgabe?
uint8_t lcd_needUpdate(void)
{
	return lcd_flags;
}

// schrittweise abarbeiten
void lcd_step(void)
{
	static uint8_t pos = 0;
	static uint8_t z = 0xFF;		// keine zeile
	static uint8_t npage = 0xFF, naddr = 0xFF;	// neue page/address setzen wenn nötig
	uint8_t limit = 0, i;
	
	while(limit < 100)
	{
		if(npage != 0xFF)
		{
			if(lcd_isBusy1() || lcd_isBusy2())
				continue;
			LCD_CS1_CS2_ON;
			LCD_CMD_MODE;
			lcd_write(LCD_CMD_SETPAGE | npage);
			npage = 0xFF;
			limit += 6;
			continue;
		}
		
		
		if(naddr != 0xFF)
		{
			if(lcd_isBusy1() || lcd_isBusy2())
				continue;
			LCD_CS1_CS2_ON;
			LCD_CMD_MODE;
			lcd_write(LCD_CMD_SETADD | naddr);
			naddr = 0xFF;
			limit += 6;
			continue;
		}		
		if(z == 0xFF)		// keine zeile in arbeit -> neue suchen
		{
			if(lcd_flags == 0)	// nichts zu tun
				return;
			
			if(lcd_isBusy1() || lcd_isBusy2())
			{
				limit += 5;
				continue;
			}					
			
			i = 1;
			for(z = 0; z < 8; z++)
			{
				if(lcd_flags & i)
					break;
				i <<= 1;
			}
			
			lcd_toBuffer(z);
			npage = z;
			naddr = 0;
			pos = 0;
			limit += 30;
		}
		else				// alte weitermachen
		{
			if(pos < 64)
			{
				LCD_CS1_ON;
			}				
			else
			{
				LCD_CS2_ON;
			}				
			
			if(lcd_isBusy_())
			{
				limit += 2;
				continue;
			}
			
			LCD_DATA_W_MODE;
			lcd_write(lcd_output_buffer[pos]);
			pos++;
			
			if(pos >= 128)
			{
				cbi(lcd_flags, (1 << z));
				z = 0xFF;
			}
			limit += 4;
		}
	}
}

// ecco logo ausgeben
void lcd_drawGecco(void)
{
	uint8_t x, y;
	uint16_t pos;
	
	for(y = 0; y < 8; y++)
	{
		while(lcd_isBusy2())
			;
		LCD_CMD_MODE;
		lcd_write(LCD_CMD_SETPAGE | y);
		
		while(lcd_isBusy2())
			;
		LCD_CMD_MODE;
		if(y == 0)
			lcd_write(LCD_CMD_SETADD | 5);
		else
			lcd_write(LCD_CMD_SETADD | 0);
		
		if(y == 0)
		{
			x = 5;
			pos = 5 * 8;
		}			
		else
		{
			x = 0;
			pos = y;
		}			
			
		for(; x < 64; x++)
		{
			while(lcd_isBusy2())
				;
			LCD_DATA_W_MODE;
			lcd_write(pgm_read_byte(&font_gecco[pos]));
			pos += 8;
		}
	}
}