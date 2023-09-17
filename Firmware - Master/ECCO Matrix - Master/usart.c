/*
	USART und Low-Level LED Ansteuerung
*/

#include <avr/io.h>
#include <util/delay.h>

#include "usart.h"
#include "master.h"
#include "led.h"

uint8_t usart_cs_mask[5] = {(1 << PD2), (1 << PD3), (1 << PD4), (1 << PD5), (1 << PD6)};

extern led_data_t led_data[5][5];

// init
void usart_init(void)
{
	//8 bit, 1 stop bit, no parity, 2x flag
	UCSR0A = 0;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	UBRR0 = 15;
}

// senden, warten bis senden fertig
void inline usart_send(uint8_t data)
{	
	while(!(UCSR0A & (1 << UDRE0)))
		;
	
	UDR0 = data;

	while(!(UCSR0A & (1 << TXC0)))
		;
}


// warten falls nötig, senden
void inline usart_send2(uint8_t data)
{
	while(!(UCSR0A & (1 << UDRE0)))
		;
	
	UDR0 = data;
}

void usart_send_led_data()
{
	uint8_t i, z;
	
	for(z = 0; z < 5; z++)
	{
		PORTD &= ~usart_cs_mask[z];
		nop; nop; nop; nop; nop; nop;
		PORTD |= usart_cs_mask[z];
		
		usart_send(USART_CMD_SETCOL);
		for(i = 0; i < 5; i++)
		{
			usart_send(led_data[z][i].r);
			usart_send(led_data[z][i].g);
			usart_send(led_data[z][i].b);
		}
		usart_send(USART_CMD_END);
		_delay_us(500);
	}
	_delay_ms(10);
	
	sCSall_ON;
	nop; nop; nop; nop; nop; nop;
	sCSall_OFF;
	
	usart_send(USART_CMD_LATCH);
	usart_send(USART_CMD_END);
	_delay_ms(1);
}