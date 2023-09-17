#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "master.h"
#include "usart.h"
#include "animation.h"
#include "lcd.h"
#include "ps2controller.h"
#include "relais.h"
#include "audio.h"
#include "ui_matrix.h"
#include "ui_lcd.h"
#include "app_base.h"
#include "app_highscore.h"

#define TEXT_DELAY		600000UL		// delay zwischen text

volatile uint32_t timestamp = 0;
uint32_t timestamp_text = 0;
uint32_t state_timeout = 0;
volatile uint8_t flags = 0;
uint8_t main_state = STATE_IDLE;

unsigned short get_seed()
{
	unsigned short seed = 0;
	unsigned short *p = (unsigned short*)(RAMEND + 1);
	extern unsigned short __heap_start;
	
	while(p >= &__heap_start + 1)
		seed ^= *(--p);
	
	return seed;
}

// timerwerte für aktionen

#define TIMER_CNT_FRAME		50
#define TIMER_CNT_TICK		100
#define TIMER_CNT_PS2INPUT	50
#define TIMER_CNT_AUDIO		16			//31


// timer 0 - 1kHz
// zeitbasis
ISR(TIMER0_COMPA_vect)
{
	static uint8_t frame_cnt = TIMER_CNT_FRAME;
	static uint8_t tick_cnt = TIMER_CNT_TICK;
	static uint8_t ps2input_cnt = TIMER_CNT_PS2INPUT;
	static uint8_t audio_cnt = TIMER_CNT_AUDIO;
	
	timestamp++;
	
	frame_cnt--;
	if(!frame_cnt)
	{
		sbi(flags, FLAG_UPDATE_FRAME);
		frame_cnt = TIMER_CNT_FRAME;
	}
	
	tick_cnt--;
	if(!tick_cnt)
	{
		sbi(flags, FLAG_TIMER_TICK);
		tick_cnt = TIMER_CNT_TICK;
	}
	
	ps2input_cnt--;
	if(!ps2input_cnt)
	{
		sbi(flags, FLAG_PS2INPUT_TICK);
		ps2input_cnt = TIMER_CNT_PS2INPUT;
	}
	
	audio_cnt--;
	if(!audio_cnt)
	{
		sbi(flags, FLAG_AUDIO_TICK);
		audio_cnt = TIMER_CNT_AUDIO;
	}		
}

// timer 1
ISR(TIMER1_COMPA_vect)
{
	AUDIO_PORT ^= (1 << AUDIO_LEFT);
	OCR1A += audio_ocr_val_l;
}

ISR(TIMER1_COMPB_vect)
{
	AUDIO_PORT ^= (1 << AUDIO_RIGHT);
	OCR1B += audio_ocr_val_r;
}

void startIdleMode(void)
{
	sbi(flags, FLAG_REDRAW_GECCO);
	cbi(flags, FLAG_IDLE_AND_LCD);
	lcd_clr();
	
	lcd_print(0, 0, "ECCO MATRIX");
	lcd_print(2, 0, "\xEA Projekt");
	lcd_print(3, 0, "  auswahl");
	lcd_print(5, 0, "\xEF Men\x81");
	
	audio_stop();
	anim_setRandom(1);
	anim_random(ANIM_PLAY_LEN);
	relais_deactivate_all();
}

int main(void)
{	
	// init DDR
	
	// LCD Datenbus
	DDRB = 0xFF;
	
	// SR und PS2 Controller
	// alles ausgang ausser PS2_DATA
	DDRC = 0xFF & ~(1 << PS2_DATA);
	
	// LCD Steuerung und Audio
	// alles ausgang
	DDRA = 0xFF;
	
	// usart und CS
	// alles ausgang ausser RX
	DDRD = 0xFF & ~(1 << DDD0);
	PORTD |= (1 << PD7);			// shift register output enable high -> daktiviert
	
	
	// Init Peripherie
	
	// LCD
	lcd_init();
	lcd_clear();
	
	// Init USART und Slaves
	usart_init();
	
	sCSall_OFF;
	_delay_ms(1);
	sCSall_ON;
	_delay_ms(1);
	sCSall_OFF;
	_delay_ms(1);
	usart_send2(USART_CMD_END);
	_delay_ms(10);
	
	led_init();	
	
	// SR/Relais
	relais_init();

	// PS2 controller
	ps2_init();
	
	// timer 0
	TCCR0A = (1 << WGM01);			// CTC
	TCCR0B = (1 << CS02);			// prescaler 256
	OCR0A = 78;
	TIMSK0 = (1 << OCIE0A);
	
	// timer 1 -> audio
	TCCR1A = 0;
	TCCR1B = 0;			// aus
	TCCR1C = 0;
	OCR1A = 0;
	OCR1B = 0;
	TIMSK1 = 0;			// auch aus
	
	// sonstiges
	flags = 0;
	timestamp_text = 0;
	srand(get_seed());
	
	anim_init();
	anim_setRandom(1);
	audio_init();
	app_highscore_init();
	
	startIdleMode();
	
	// WDT
	wdt_enable(WDTO_500MS);	

	// Interrupts freigeben
	sei();
	
	while(1)
	{
		wdt_reset();
		 
		if(isset(flags, FLAG_TIMER_TICK))			// wichtige sachen machen bei timer tick
		{
			relais_tick();
			ps2_vibration_step();
			
			cbi(flags, FLAG_TIMER_TICK);
		}
		else if(isset(flags, FLAG_AUDIO_TICK))
		{
			audio_step();
			cbi(flags, FLAG_AUDIO_TICK);
		}
		else if(isset(flags, FLAG_PS2INPUT_TICK))		// controller auslesen
		{
			if(ps2_poll())	// eingabe ok
			{
				switch(main_state)
				{
				case STATE_IDLE:
					if(ps2_data.btn1_down & PS2_START)		// in menü
					{
						main_state = STATE_UI_MENU;
						state_timeout = timestamp + STATE_TIMEOUT_LEN;
						LCD_LIGHT_ON;
						relais_deactivate_all();
						uil_start();
					}
					else if(ps2_data.btn1_down != 0 || ps2_data.btn2_down != 0)	// in matrix-ui-modus
					{
						if(isset(flags, FLAG_IDLE_AND_LCD))
						{
							main_state = STATE_UI_MATRIX;
							state_timeout = timestamp + STATE_TIMEOUT_LEN;
							LCD_LIGHT_ON;
							cbi(flags, FLAG_IDLE_AND_LCD);
							uim_start();
						}
						else
						{
							sbi(flags, FLAG_IDLE_AND_LCD);
							LCD_LIGHT_ON;
							state_timeout = timestamp + 10000;
						}
						
					}
					
					break;
					
				case STATE_UI_MATRIX:
					if(ps2_data.btn1_down & PS2_START)		// in menü
					{
						main_state = STATE_UI_MENU;
						state_timeout = timestamp + STATE_TIMEOUT_LEN;
						LCD_LIGHT_ON;
						relais_deactivate_all();
						uil_start();
						break;
					}
					else if(ps2_data.btn2_down & PS2_CIRCLE)
					{
						state_timeout = timestamp;
						break;
					}
					
					if(ps2_data.btn1_down != 0 || ps2_data.btn2_down != 0)
						state_timeout = timestamp + STATE_TIMEOUT_LEN;
				
					uim_input();
					break;
					
				case STATE_UI_MENU:
					if(ps2_data.btn1_down != 0 || ps2_data.btn2_down != 0)
						state_timeout = timestamp + STATE_TIMEOUT_LEN;
					
					if(ps2_data.btn1_down & PS2_START)		// menü verlassen
					{
						state_timeout = timestamp;
						audio_stop();
					}
					else
					{
						uil_input();
					}
					break;
					
				case STATE_APP:
					if(ps2_data.btn1_down != 0 || ps2_data.btn2_down != 0)
						state_timeout = timestamp + STATE_TIMEOUT_LEN;
						
					app_input();
					break;
				
				default:
					main_state = STATE_IDLE;
					break;
				} // switch
				
			} // if poll		
			
			cbi(flags, FLAG_PS2INPUT_TICK);
		}
		
		if(isset(flags, FLAG_UPDATE_FRAME))			// bild ausgeben
		{
			if(main_state == STATE_APP)
			{
				app_draw();
			}
			else
			{
				anim_frame();

				led_copy_buff();

				if(main_state == STATE_UI_MATRIX)
				{
					led_ae_darken();
					uim_overlay();
				}
				else if(isset(flags, FLAG_SCROLL_TEXT))
				{
					led_font_step();
					led_ae_darken();
					led_font_merge();
				}
			}
			
			usart_send_led_data();	
			cbi(flags, FLAG_UPDATE_FRAME);	
		}
		
		else if(lcd_needUpdate())
		{
			lcd_step();
		}
		
		else										// oder sonst was
		{
			if(main_state != STATE_IDLE || isset(flags, FLAG_IDLE_AND_LCD))
			{
				if(timestamp >= state_timeout)			// idle gehen
				{
					LCD_LIGHT_OFF;
					lcd_clr();
					main_state = STATE_IDLE;
					startIdleMode();
				}
			}
			else if(isset(flags, FLAG_REDRAW_GECCO))	// gecco zeichnen
			{
				lcd_drawGecco();
				cbi(flags, FLAG_REDRAW_GECCO);
			}
			else if(timestamp_text <= timestamp)	// zufälliger text
			{
				led_font_start_random();
				timestamp_text = timestamp + TEXT_DELAY + (((uint32_t)rand()) << 8) % TEXT_DELAY;
			}
		} // arbeit?
		
	}
	
}