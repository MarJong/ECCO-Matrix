/*
 * slave.h
 *
 * Created: 20.06.2013 19:14:54
 *  Author: Marcel
 */ 


#ifndef SLAVE_H_
#define SLAVE_H_

#define FLAG_UPDATE		0x01		// eingangs -> ausgabebuffer
#define FLAG_EXECUTE	0x02		// usart befehl ausführen
#define FLAG_OUTPUT		0x04		// led daten in shift register
#define FLAG_TIME		0x08		// viel freizeit für usart

#define nop				asm volatile ("nop")

#define sbi(port, pin)		((port) |= (1 << (pin)))
#define cbi(port, pin)		((port) &= ~(1 << (pin)))

// schieberegister anschluss
#define SR_PORT			PORTB
#define SR_CLK			PB7
#define SR_DO			PB6
#define SR_LATCH		PB4
#define SR_ENABLE		PB3

#define ENABLE_RX		(UCSRB |=  ((1 << RXCIE) | (1 << RXEN)))
#define DISABLE_RX		(UCSRB &= ~((1 << RXCIE) | (1 << RXEN)))

#define USART_BUFFER_LEN		32

// befehle
#define CMD_ALL_RGB				0x10
#define CMD_ROW_RGB				0x11
#define CMD_FIELD_RGB			0x12

#define CMD_LATCH				0x20

#define CMD_DISABLE				0x30
#define CMD_ENABLE				0x31

#define USART_NEXT_CMD			0xF0
#define USART_EXECUTE			0xFE

#define RESP_ACK				0xAA

// länge kürzester pwm cycle
#define TIMER_BASE_OCR			783

// sonstiges
typedef struct 
{
	uint8_t r, g, b;
} led_data_t;


#endif /* SLAVE_H_ */