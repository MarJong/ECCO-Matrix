#ifndef USART_H_
#define USART_H_

#include "led.h"

#define sCSall_ON	(PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6)))
#define sCSall_OFF	(PORTD |= ((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6)))

#define USART_CMD_SETCOL		0x10
#define USART_CMD_LATCH			0x20
#define USART_CMD_ON			0x30
#define USART_CMD_OFF			0x31
#define USART_CMD_SYNC			0xF0
#define USART_CMD_END			0xFE
#define USART_CMD_ACK			0xAA

// init
void usart_init(void);

// data schicken + warten bis fertig
void usart_send(uint8_t data);
// warten falls nötig + schicken
void usart_send2(uint8_t data);

// schickt komplettes led_data paket
void usart_send_led_data();

#endif /* USART_H_ */