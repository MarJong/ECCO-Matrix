#ifndef LCD_H_
#define LCD_H_

#define LCD_DATA_PORT_IN		DDRB = 0x00
#define LCD_DATA_PORT_OUT		DDRB = 0xFF

#define LCD_LIGHT_ON			PORTC |= (1 << PC3)
#define LCD_LIGHT_OFF			PORTC &= ~(1 << PC3)

#define LCD_DELAY_500ns	{nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;}
#define LCD_DELAY_300ns	{nop; nop; nop; nop; nop; nop;}
#define LCD_CS1_ON		{sbi(LCD_PORT, (1 << LCD_CS1)); cbi(LCD_PORT, (1 << LCD_CS2)); LCD_DELAY_500ns;}
#define LCD_CS2_ON		{sbi(LCD_PORT, (1 << LCD_CS2)); cbi(LCD_PORT, (1 << LCD_CS1)); LCD_DELAY_500ns;}
#define LCD_CS1_CS2_ON	{sbi(LCD_PORT, (1 << LCD_CS1)); sbi(LCD_PORT, (1 << LCD_CS2)); LCD_DELAY_500ns;}
#define LCD_CS_OFF		{cbi(LCD_PORT, (1 << LCD_CS1)); cbi(LCD_PORT, (1 << LCD_CS2)); LCD_DELAY_500ns;}
	
#define LCD_CMD_MODE	{cbi(LCD_PORT, (1 << LCD_RS)); cbi(LCD_PORT, (1 << LCD_RW)); LCD_DELAY_500ns;}
#define LCD_STATUS_MODE	{cbi(LCD_PORT, (1 << LCD_RS)); sbi(LCD_PORT, (1 << LCD_RW)); LCD_DELAY_500ns;}
	
#define LCD_DATA_R_MODE	{sbi(LCD_PORT, (1 << LCD_RS)); sbi(LCD_PORT, (1 << LCD_RW)); LCD_DELAY_500ns;}
#define LCD_DATA_W_MODE	{sbi(LCD_PORT, (1 << LCD_RS)); cbi(LCD_PORT, (1 << LCD_RW)); LCD_DELAY_500ns;}

#define LCD_CLK_DELAY	{nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;}

#define LCD_CMD_ON		0x3F
#define LCD_CMD_OFF		0x3E
#define LCD_CMD_SETADD	0x40
#define LCD_CMD_SETPAGE 0xB8
#define LCD_CMD_DISP	0xC0

#define LCD_ATTR_INVERT		0x01		// invertieren (komplette zeile)
#define LCD_ATTR_CENTER		0x02		// zentrieren
#define LCD_ATTR_INVERT_T	0x04		// invertieren (nur text)

// init
void lcd_init(void);

// alles löschen
void lcd_clear(void);

// schrittweise abarbeiten
void lcd_step(void);

// benötigt ausgabe?
uint8_t lcd_needUpdate(void);

// text ausgeben
void lcd_print(uint8_t zeile, uint8_t attr, char *str, ...);
// löschen für im-programm
void lcd_clr(void);

// ecco logo ausgeben
void lcd_drawGecco(void);

#endif /* LCD_H_ */