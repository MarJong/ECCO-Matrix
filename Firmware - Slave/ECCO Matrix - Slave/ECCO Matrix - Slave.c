/*
 * ECCO_Matrix___Slave.c
 *
 * Created: 19.06.2013 16:17:18
 *  Author: Marcel
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "slave.h"

// daten für led werte
// led_data = outputbuffer mit internen werten
// led_data_buffer = eingangsbuffer mit externen werten
volatile led_data_t led_data[5], led_data_buffer[5];

// usart empfangsbuffer
char volatile usart_buffer[USART_BUFFER_LEN];
uint8_t volatile usart_pos;

// flags
uint8_t volatile flags;

// high und low output byte. high geht zuerst raus. msb first
uint8_t volatile output_h, output_l;

// PWM zähler
uint8_t pwm_step = 0;

// FORMEL 2!
// 5 bit input

#define COLOR_NUM			32
// Tabelle für Umrechnung Farbe -> Anzahl Schritte zwischen PWM-Timer Aufrufen
const uint8_t PROGMEM pwm_step_mod[COLOR_NUM] =	{
													1, 1, 1, 1, 1, 2, 1, 2, 2, 3,
													2, 3, 3, 4, 4, 4, 5, 6, 7, 7,
													8, 9, 10, 11, 13, 14, 16, 18, 21, 22,
													26, 28
												};

#define ZEIT_AB		16			// ab diesem pwm zyklus andere sachen machen
#define COL_MASK	0x1F		// 5bit farbe - 0001 1111

/*
 * USART
 */

void usart_send(char c)
{
	while(!(UCSRA & (1 << UDRE)))
		;
	
	UDR = c;
}

void usart_execute(uint8_t data_len)
{
	uint8_t i, pos = 0;
		
	while(pos < data_len)
	{
		switch(usart_buffer[pos])
		{
			case CMD_ALL_RGB:	// 5x RGB		daten für komplette zeile
				led_data_buffer[0].r = usart_buffer[pos + 1] & COL_MASK;
				led_data_buffer[0].g = usart_buffer[pos + 2] & COL_MASK;
				led_data_buffer[0].b = usart_buffer[pos + 3] & COL_MASK;
				
				led_data_buffer[1].r = usart_buffer[pos + 4] & COL_MASK;
				led_data_buffer[1].g = usart_buffer[pos + 5] & COL_MASK;
				led_data_buffer[1].b = usart_buffer[pos + 6] & COL_MASK;
				
				led_data_buffer[2].r = usart_buffer[pos + 7] & COL_MASK;
				led_data_buffer[2].g = usart_buffer[pos + 8] & COL_MASK;
				led_data_buffer[2].b = usart_buffer[pos + 9] & COL_MASK;
				
				led_data_buffer[3].r = usart_buffer[pos + 10] & COL_MASK;
				led_data_buffer[3].g = usart_buffer[pos + 11] & COL_MASK;
				led_data_buffer[3].b = usart_buffer[pos + 12] & COL_MASK;
				
				led_data_buffer[4].r = usart_buffer[pos + 13] & COL_MASK;
				led_data_buffer[4].g = usart_buffer[pos + 14] & COL_MASK;
				led_data_buffer[4].b = usart_buffer[pos + 15] & COL_MASK;
				
				pos += 16;
				break;
		
			case CMD_ROW_RGB:	// RGB		komplette zeile auf die farbe
				for(i = 0; i < 5; i++)
				{
					led_data_buffer[i].r = usart_buffer[pos + 1] & COL_MASK;
					led_data_buffer[i].g = usart_buffer[pos + 2] & COL_MASK;
					led_data_buffer[i].b = usart_buffer[pos + 3] & COL_MASK;
				}
							
				pos += 4;
				break;
			
			case CMD_FIELD_RGB:	//bRGB		setzt feld b auf farbe
				i = usart_buffer[pos + 1];
				
				if(i < 5)
				{
					led_data_buffer[i].r = usart_buffer[pos + 2] & COL_MASK;
					led_data_buffer[i].g = usart_buffer[pos + 3] & COL_MASK;
					led_data_buffer[i].b = usart_buffer[pos + 4] & COL_MASK;
				}
				
				pos+= 5;
				break;
			
			case CMD_LATCH:	// -		latch
				flags |= FLAG_UPDATE;
				pos += 1;
				break;
		
			case CMD_DISABLE:	// -		zeile abschalten
				sbi(SR_PORT, SR_ENABLE);
				pos += 1;
				break;
			
			case CMD_ENABLE:	// -		zeile anschalten
				cbi(SR_PORT, SR_ENABLE);
				pos += 1;
				break;
			
			default:
				return;
		}
		
		if(usart_buffer[pos] != USART_NEXT_CMD)
		{
			return;
		}
		pos++;
	}
	
}

ISR(USART_RX_vect)
{
	if(usart_pos < USART_BUFFER_LEN)
	{
		usart_buffer[usart_pos] = UDR;
		usart_pos++;
		if(usart_buffer[usart_pos - 1] == USART_EXECUTE)
		{
			flags |= FLAG_EXECUTE;
			DISABLE_RX;
			usart_send(RESP_ACK);
		}			
	}
	else
	{
		DISABLE_RX;
	}
}

/*
 * INT0 (CS vom USART) 
 */

ISR(INT0_vect)
{
/*
	if(PIND & (1 << PD2))	// high -> USART deaktivieren + ausführen
	{
	}
*/
//	if(!((PIND & (1 << PD2))))		// low -> USART aktiveren
//	{
		usart_pos = 0;
		ENABLE_RX;
//	}
}

/*
 * hauptprogramm
 */

// init
//*	3G 3B 4R 4G 4B 5R 5G 5B | NC 1R 1G 1B 2R 2G 2B 3R
void init(void)
{
	uint8_t i;
	usart_pos = 0;
	flags = 0;
	
	output_h = 0xFF;
	output_l = 0xFF;
	
	for(i = 0; i < 5; i++)
	{
		led_data[i].r = 0;
		led_data[i].g = 0;
		led_data[i].b = 0;
	
		led_data_buffer[i].r = 0;
		led_data_buffer[i].g = 0;
		led_data_buffer[i].b = 0;
	}
	
	for(i = 0; i < USART_BUFFER_LEN; i++)
	{
		usart_buffer[i] = 0;
	}
}

// daten ins schieberegister ausgeben
void inline sr_output(void)
{
	int8_t i, temp;
	
	temp = output_h;
	i = 8;
	
	do {
		if(temp & 0x80)
			sbi(SR_PORT, SR_DO);
		else
			cbi(SR_PORT, SR_DO);
		
		sbi(SR_PORT, SR_CLK);
		cbi(SR_PORT, SR_CLK);
		
		temp <<= 1;	
	} while(--i);
	
	temp = output_l;
	i = 8;
	
	do {
		if(temp & 0x80)
			sbi(SR_PORT, SR_DO);
		else
			cbi(SR_PORT, SR_DO);
		
		sbi(SR_PORT, SR_CLK);
		cbi(SR_PORT, SR_CLK);
		
		temp <<= 1;
	} while(--i);
	
	sbi(SR_PORT, SR_LATCH);
	cbi(SR_PORT, SR_LATCH);
}

/*
*	schieberegister datenorganisation
*	3G 3B 4R 4G 4B 5R 5G 5B | NC 1R 1G 1B 2R 2G 2B 3R
*/
// PWM Timer
ISR(TIMER1_COMPA_vect)
{
	uint8_t mod = pgm_read_byte_near(&pwm_step_mod[pwm_step]); // timer aufrufe bis nächste mögliche änderung
	uint8_t i = 0;
	
	if(pwm_step == 0)
	{
		flags &= ~FLAG_TIME;
		output_h = 0xFF;
		output_l = 0xFF;;
	}
	
	// neue timer grenze setzen
	OCR1A = TIMER_BASE_OCR * mod;
	
	// ausgabe prüfen + ändern
	if(led_data[2].g < pwm_step)
		cbi(output_h, 7);
	if(led_data[2].b < pwm_step)
		cbi(output_h, 6);
	if(led_data[3].r < pwm_step)
		cbi(output_h, 5);
	if(led_data[3].g < pwm_step)
		cbi(output_h, 4);
	if(led_data[3].b < pwm_step)
		cbi(output_h, 3);
	if(led_data[4].r < pwm_step)
		cbi(output_h, 2);
	if(led_data[4].g < pwm_step)
		cbi(output_h, 1);
	if(led_data[4].b < pwm_step)
		cbi(output_h, 0);

//	pin nc	
//	if(led_data[2].g < pwm_step)
//		cbi(output_l, 7);
	if(led_data[0].r < pwm_step)
		cbi(output_l, 6);
	if(led_data[0].g < pwm_step)
		cbi(output_l, 5);
	if(led_data[0].b < pwm_step)
		cbi(output_l, 4);
	if(led_data[1].r < pwm_step)
		cbi(output_l, 3);
	if(led_data[1].g < pwm_step)
		cbi(output_l, 2);
	if(led_data[1].b < pwm_step)
		cbi(output_l, 1);
	if(led_data[2].r < pwm_step)
		cbi(output_l, 0);
	
	flags |= FLAG_OUTPUT;	
	
	pwm_step++;
	
	if(pwm_step == ZEIT_AB)	// ab jetzt genug zeit für andere sachen
	{
		flags |= FLAG_TIME;
	}	
	else if(pwm_step >= COLOR_NUM)
	{
		pwm_step = 0;

		if(flags & FLAG_UPDATE)
		{
			// farbwerte aus buffer übernehmen
			for(i = 0; i < 5; i++)
			{
				led_data[i].r = led_data_buffer[i].r;
				led_data[i].g = led_data_buffer[i].g;
				led_data[i].b = led_data_buffer[i].b;
			}
			
			flags &= ~FLAG_UPDATE;
		}
	}
}

// main
int main(void)
{
	// Datenrichtungen
	DDRB = 0xFF;				// alles ausgang
	DDRD = (1 << DDD1);			// TX ausgang

	PORTD = (1 << PD2);			// pull-up an int0
	
	// init usart
	// rx interrupt nur bei bedarf aktiv
	// 1 stop bit, kein parity, 8 bit daten
	UBRRH = 0;
	UBRRL = 15;
	UCSRA = 0;
	UCSRB = (1 << TXEN);
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
	 
	// init INT0
	MCUCR |= (1 << ISC01);		// fallende flanke
	GIMSK |= (1 << INT0);		// aktivieren
	
	// software PWM timer; 8 bit pwm
	// 255 PWM Schritte, 100Hz = 25500 aufrufe/sek
	// 20MHz / 25500 aufrufe/sek = 784 zyklen/aufruf
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS10);	// CTC, pre-scaler 1
	TCCR1C = 0;
	OCR1A = TIMER_BASE_OCR;				// -> slave.h
	TIMSK |= (1 << OCIE1A); 
	
	// init buffer
	init();
	
	// interrupts aktivieren
	sei();
	
	// watchdog
	wdt_enable(WDTO_15MS);
	
    while(1)
    {	
		wdt_reset();
		
		if(flags & FLAG_OUTPUT)		// output an shift register schicken
		{
			flags &= ~FLAG_OUTPUT;
			sr_output();
		}
		
		if(flags & FLAG_TIME)			// usart hat daten und genug zeit zum usart auswerten
		{
			if(flags & FLAG_EXECUTE)
			{
				usart_execute(usart_pos);
				flags &= ~FLAG_EXECUTE;
			}
		}			
    }
}