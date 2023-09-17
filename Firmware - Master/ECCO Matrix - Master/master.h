/*
	Port Definitionen
	Allgemeine Hilfsfunktionen
*/

#ifndef MASTER_H_
#define MASTER_H_

#define STATE_TIMEOUT_LEN		180000UL		// nach x msek wieder idle

#define nop				asm volatile ("nop")

#define sbi(x, y)		((x) |= (y))
#define cbi(x, y)		((x) &= ~(y))
#define isset(x, y)		((x) & (y))

// Schieberegister für Relais
#define SR_PORT			PORTC
#define SR_LATCH		PC1
#define SR_CLOCK		PC2
#define SR_DATA			PC0
//outputenable portd pd7

// PS2 Controller
#define PS2_PORT		PORTC
#define PS2_PIN			PINC
#define PS2_ATT			PC4
#define PS2_CMD			PC5
#define PS2_DATA		PC6
#define PS2_CLOCK		PC7

// LCD - Daten
#define LCD_DATA		PORTB
#define LCD_DATA_PIN	PINB
// LCD - Steuerung
#define LCD_PORT		PORTA
#define LCD_CS2			PA0
#define LCD_CS1			PA1
#define LCD_E			PA2
#define LCD_RW			PA3
#define LCD_RS			PA4
#define LCD_RESET		PA5
// beleuchtung portc pc3

// Audio
#define AUDIO_PORT		PORTA
#define AUDIO_LEFT		PA6
#define AUDIO_RIGHT		PA7

// Slaves -> usart.h

// flags
#define FLAG_UPDATE_FRAME		0x01		// update für bild/led (berechnen)
#define FLAG_TIMER_TICK			0x02		// 10Hz timer
#define FLAG_PS2INPUT_TICK		0x04		// controller auslesen
#define FLAG_AUDIO_TICK			0x08		// audiokram
#define FLAG_TRANSFER_FRAME		0x10		// neues bild übertragen
#define FLAG_SCROLL_TEXT		0x20		// lauftext läuft
#define FLAG_IDLE_AND_LCD		0x40		// idle state mit lcd backlight an
#define FLAG_REDRAW_GECCO		0x80		// ecco logo neu zeichnen

// state
#define STATE_IDLE				1			// idle modus; zufällige animationen abspielen
#define STATE_UI_MATRIX			2			// interaktion mit matrix/schaukästen
#define STATE_UI_MENU			3			// interaktion mit lcd/menü
#define STATE_APP				4			// APP!

// globale variablen

extern volatile uint32_t timestamp;

#endif /* MASTER_H_ */