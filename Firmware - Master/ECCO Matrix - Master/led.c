/*
	LED Grundsystem
*/

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdlib.h>
#include <string.h>

#include "master.h"
#include "animation.h"
#include "led.h"

led_data_t led_data[5][5], led_data_buffer[5][5];
led_font_data_t led_text;

extern volatile uint8_t flags;

/* 
	LED Schrift 3x5
	- 1 2 3
	1 b b b		LSB
	2 y y y
	3 t t t
	4 e e e
	5 1 2 3		MSB
	zeichen	' '	...	Z
	ascii	32	...	90
*/

const uint8_t PROGMEM led_font_3x5[59][3] = {
	// ' '		!			"			#				$			%			&		'			(			)
	{0, 0, 0}, {0, 23, 0}, {7, 0, 7}, {10, 31, 10}, {0, 0, 0}, {9, 4, 18}, {11, 21, 11}, {0, 7, 0}, {14, 17, 0}, {0, 17, 14},
	// *			+				,			-			.			/		0				1				2			3	
	{21, 14, 21}, {4, 14, 4}, {16, 8, 0}, {4, 4, 4}, {24, 24, 0}, {0, 0, 0}, {31, 17, 31}, {18, 31, 16}, {29, 21, 23}, {21, 21, 31},
	//	4			5			6				7			8				9			:			;			<			=
	{7, 4, 31}, {23, 21, 29}, {31, 21, 29}, {3, 1, 31}, {31, 21, 31}, {23, 21, 31}, {0, 10, 0}, {16, 10, 0}, {4, 10, 17}, {10, 10, 10},
	// >				?		@			A				B			C			D				E			F			G
	{17, 10, 4}, {1, 21, 2}, {31, 17, 21}, {30, 5, 30}, {31, 21, 10}, {31, 17, 17}, {31, 17, 14}, {31, 21, 21}, {31, 5, 5}, {15, 21, 13},
	//	H			I				J			K				L			M			N			O				P			Q
	{31, 4, 31}, {17, 31, 17}, {25, 17, 31}, {31, 4, 27}, {31, 16, 16}, {31, 6, 31}, {31, 1, 31}, {31, 17, 31}, {31, 5, 2}, {6, 9, 22},
	//	R			S				T		U				V			W				X			Y			Z
	{31, 5, 26}, {18, 21, 9}, {1, 31, 1}, {31, 16, 31}, {15, 16, 15}, {31, 12, 31}, {27, 4, 27}, {3, 28, 3}, {25, 21, 19}
};

// je zeile 1 byte
uint8_t led_font_overlay[5];

// init led data
void led_init(void)
{
	uint8_t i, j;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			led_data[i][j].r = 0;
			led_data[i][j].g = 0;
			led_data[i][j].b = 0;
			
			led_data_buffer[i][j].r = 0;
			led_data_buffer[i][j].g = 0;
			led_data_buffer[i][j].b = 0;
		}
		
		led_font_overlay[i] = 0;
	}
	led_text.pos = 0;
	led_text.subpos = 0;
	led_text.text[0] = 0;
	led_text.del = LED_FONT_DELAY;
}

void led_copy_buff(void)
{
	uint8_t i, j;
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			led_data[i][j].r = led_data_buffer[i][j].r;
			led_data[i][j].g = led_data_buffer[i][j].g;
			led_data[i][j].b = led_data_buffer[i][j].b;
		}
	}
}

/*
	Basis LED Funktionen
*/

// komplettes feld auf RGB setzen
void led_setRGB(uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t i, j;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			led_data_buffer[i][j].r = r;
			led_data_buffer[i][j].g = g;
			led_data_buffer[i][j].b = b;
		}
	}
}

// kopiert farbe src nach dest
void led_copyCol(led_data_t *dest, led_data_t *src)
{
	dest->r = src->r;
	dest->g = src->g;
	dest->b = src->b;
}

// alles nach 1 links
void led_shiftLeft(void)
{
	uint8_t x, y;
	
	for(x = 0; x < 4; x++)
	{
		for(y = 0; y < 5; y++)
		{
			led_data_buffer[x][y].r = led_data_buffer[x + 1][y].r;
			led_data_buffer[x][y].g = led_data_buffer[x + 1][y].g;
			led_data_buffer[x][y].b = led_data_buffer[x + 1][y].b;
		}
	}
}

// alles nach 1 rechts
void led_shiftRight(void)
{
	uint8_t x, y;
	
	for(x = 4; x > 0; x--)
	{
		for(y = 0; y < 5; y++)
		{
			led_data_buffer[x][y].r = led_data_buffer[x - 1][y].r;
			led_data_buffer[x][y].g = led_data_buffer[x - 1][y].g;
			led_data_buffer[x][y].b = led_data_buffer[x - 1][y].b;
		}
	}
}

// alles nach 1 oben
void led_shiftUp(void)
{
	uint8_t x, y;
	
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 4; y++)
		{
			led_data_buffer[x][y].r = led_data_buffer[x][y + 1].r;
			led_data_buffer[x][y].g = led_data_buffer[x][y + 1].g;
			led_data_buffer[x][y].b = led_data_buffer[x][y + 1].b;
		}
	}
}

// alles nach 1 unten
void led_shiftDown(void)
{
	uint8_t x, y;
	
	for(x = 0; x < 5; x++)
	{
		for(y = 4; y > 0; y--)
		{
			led_data_buffer[x][y].r = led_data_buffer[x][y - 1].r;
			led_data_buffer[x][y].g = led_data_buffer[x][y - 1].g;
			led_data_buffer[x][y].b = led_data_buffer[x][y - 1].b;
		}
	}
}

// farben abdunkeln after-effect
void led_ae_darken(void)
{
	uint8_t x, y;
	
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			led_data[x][y].r >>= 1;
			led_data[x][y].g >>= 1;
			led_data[x][y].b >>= 1;
		}
	}	
}

// (mehr oder weniger) kräftige zufallsfarbe
void led_getCol(led_data_t *col)
{
	uint8_t y;
	
	y = rand() % 3;
		
	switch(y)
	{
	case 0:
		col->r = LED_COL_MAX;
		col->g = rand() % LED_COL_MAX;
		col->b = rand() % LED_COL_MAX;
		break;
			
	case 1:
		col->g = LED_COL_MAX;
		col->r = rand() % LED_COL_MAX;
		col->b = rand() % LED_COL_MAX;
		break;
			
	default:
		col->b = LED_COL_MAX;
		col->g = rand() % LED_COL_MAX;
		col->r = rand() % LED_COL_MAX;
		break;
	}
}

// HSV ohne SV
void led_hue(uint8_t hue, led_data_t *led)
{
	uint16_t subhue;
	
	if(hue > 251)
	{
		hue = 251;
	}
	
	subhue = (uint16_t)(hue % 42) * LED_COL_MAX / 41;
	hue /= 42;
	
	/*
	sat = max, val = max
	y1 = 0;
	y2 = LED_COL_MAX - subhue;
	y3 = LED_COL_MAX - (LED_COL_MAX - subhue);
	*/
	
	switch(hue)
	{
		case 0:
			led->r = LED_COL_MAX;
			led->g = LED_COL_MAX - (LED_COL_MAX - subhue);
			led->b = 0;
			break;
			
		case 1:
			led->r = LED_COL_MAX - subhue;
			led->g = LED_COL_MAX;
			led->b = 0;
			break;
		
		case 2:
			led->r = 0;
			led->g = LED_COL_MAX;
			led->b = LED_COL_MAX - (LED_COL_MAX - subhue);
			break;
			
		case 3:
			led->r = 0;
			led->g = LED_COL_MAX - subhue;
			led->b = LED_COL_MAX;
			break;
			
		case 4:
			led->r = LED_COL_MAX - (LED_COL_MAX - subhue);
			led->g = 0;
			led->b = LED_COL_MAX;
			break;
		
		default:
			led->r = LED_COL_MAX;
			led->g = 0;
			led->b = LED_COL_MAX - subhue;
			break;
	}
}

/*
	LED Font System
*/
#define LED_FONT_NUM	6
const char PROGMEM led_string_table[LED_FONT_NUM][LED_FONT_MAX_LEN] =
{
	"HALLO",
	"ECCO",
	"BTU",
	"FEM",
	"IMT",
	"FABLAB"
};

void led_font_start(char *text, uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t i, len;
	
	len = strlen(text);
	if(len > LED_FONT_MAX_LEN)
		return;
	
	led_text.col.r = r & LED_COL_MAX;
	led_text.col.g = g & LED_COL_MAX;
	led_text.col.b = b & LED_COL_MAX;
	strcpy(led_text.text, text);
	
	led_text.pos = 0;
	led_text.subpos = 0;
	
	for(i = 0; i < 5; i++)
		led_font_overlay[i] = 0;
		
	sbi(flags, FLAG_SCROLL_TEXT);
}

void led_font_start_random()
{
	char temp[LED_FONT_MAX_LEN], c;
	uint8_t r, g ,b, pos, t;
	
	pos = rand() % LED_FONT_NUM;
	t = rand() % 3;
	
	if(t == 0)
	{
		r = LED_COL_MAX;
		g = rand() % LED_COL_MAX;
		b = rand() % LED_COL_MAX;
	}
	else if(t == 1)
	{
		g = LED_COL_MAX;
		r = rand() % LED_COL_MAX;
		b = rand() % LED_COL_MAX;
	}
	else
	{
		b = LED_COL_MAX;
		g = rand() % LED_COL_MAX;
		r = rand() % LED_COL_MAX;
	}
	
	t = 0;
	do {
		c = pgm_read_byte(&led_string_table[pos][t]);
		temp[t] = c;
		t++;
	} while(c != '\0');
	
	led_font_start(temp, r, g, b);
}

uint8_t led_font_step(void)
{
	uint8_t i;
	char c;
	
	if(led_text.del)
	{
		led_text.del--;
		return 1;
	}
	led_text.del = LED_FONT_DELAY;
	
	if(led_text.text[led_text.pos])
	{
		if(led_text.text[led_text.pos] < ' ' || led_text.text[led_text.pos] > 'Z')
			return 0;
	
		c = pgm_read_byte(&led_font_3x5[led_text.text[led_text.pos] - ' '][led_text.subpos]);
	}
	else
	{
		c = 0xFF;
	}		
	
	// overlay 1 nach links und neuen pixel anfügen
	for(i = 0; i < 5; i++)
	{
		led_font_overlay[i] <<= 1;
		
		if(c == 0xFF)	// am ende -> nur noch shiften
			continue;
		
		if(led_text.subpos < 3) // zeichen 3 pixel breit, 4 = leer 
		{
			if(c & (1 << i))
				led_font_overlay[i] |= 1;
		}
	}
	
	led_text.subpos++;
	if(led_text.subpos >= 4)
	{
		led_text.subpos = 0;
		led_text.pos++;
	}
	
	if(c == 0xFF && !(led_font_overlay[0] & 0x1F) && !(led_font_overlay[1] & 0x1F) && !(led_font_overlay[2] & 0x1F) && 
		!(led_font_overlay[3] & 0x1F) && !(led_font_overlay[4] & 0x1F))
	{
		cbi(flags, FLAG_SCROLL_TEXT);
		return 0;	// -> string ende
	}
	
	return 1;
}

void led_font_merge()
{
	int8_t x, y;
	
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			if(led_font_overlay[y] & (1 << (4 - x)))
			{
				led_data[x][y].r = led_text.col.r;
				led_data[x][y].g = led_text.col.g;
				led_data[x][y].b = led_text.col.b;
			}
		}
	}
}

/*
void led_font_test_char(char c, uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t buf, i, y;
	
	if(c < ' ' || c > 'Z')
		return;
	
	for(i = 0; i < 3; i++)
	{
		buf = pgm_read_byte(&led_font_3x5[c - ' '][i]);
		
		for(y = 0; y < 5; y++)
		{
			if(buf & (1 << y))
			{
				led_data[i + 1][y].r = r;
				led_data[i + 1][y].g = g;
				led_data[i + 1][y].b = b;
			}
		}
	}
}
*/