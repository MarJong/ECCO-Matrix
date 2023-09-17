#include <avr/io.h>
#include <util/delay.h>

#include "ps2controller.h"
#include "master.h"
#include "lcd.h"

ps2_data_t ps2_data;

uint8_t ps2_vib_cnt;

uint8_t ps2_transfer_data(uint8_t data)
{
	uint8_t i, ret = 0, mask;
	
	mask = 1;
	for(i = 0; i < 8; i++)
	{
		if(data & mask)				// (1 << i)
			PS2_PORT |= (1 << PS2_CMD);
		else
			PS2_PORT &= ~(1 << PS2_CMD);
		
		nop; nop; nop; nop; nop;
		
		PS2_PORT &= ~(1 << PS2_CLOCK);
		_delay_us(5);
		PS2_PORT |= (1 << PS2_CLOCK);
		
		nop; nop; nop; nop; nop;
		
		if(PS2_PIN & (1 << PS2_DATA))
			ret |= mask;		// (1 << i)
		
		mask <<= 1;
	}
	PS2_PORT |= (1 << PS2_CMD);
	
	_delay_us(20);
	
	return ret;
}

void ps2_init(void)
{
	uint8_t check = 0;

	PS2_PORT |= ((1 << PS2_DATA) | (1 << PS2_CMD) | (1 << PS2_CLOCK) | (1 << PS2_ATT));
	
	ps2_data.btn1 = 0xFF;
	ps2_data.btn2 = 0xFF;
	ps2_data.last_btn1 = 0xFF;
	ps2_data.last_btn2 = 0xFF;
	ps2_data.btn1_down = 0;
	ps2_data.btn1_up = 0;
	ps2_data.btn2_down = 0;
	ps2_data.btn2_up = 0;
	
	ps2_data.lx = PS2_ANALOG_MID;
	ps2_data.ly = PS2_ANALOG_MID;
	ps2_data.rx = PS2_ANALOG_MID;
	ps2_data.ry = PS2_ANALOG_MID;
	ps2_data.vib1 = 0;
	ps2_data.vib2 = 0;
	
	// init ps2 controller in analog mode
	do 
	{	
		// poll
		PS2_PORT &= ~(1 << PS2_ATT);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0x42);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		_delay_ms(1);
		PS2_PORT |= (1 << PS2_ATT);
		_delay_ms(10);
		
		// in config mode gehen
		PS2_PORT &= ~(1 << PS2_ATT);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0x43);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0x00);
		_delay_ms(1);
		PS2_PORT |= (1 << PS2_ATT);
		_delay_ms(10);
		
		// in analog mode setzen
		PS2_PORT &= ~(1 << PS2_ATT);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0x44);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0x03);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		_delay_ms(1);
		PS2_PORT |= (1 << PS2_ATT);
		_delay_ms(10);
	
		// vibrator aktivieren
		PS2_PORT &= ~(1 << PS2_ATT);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0x4D);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0xFF);
		ps2_transfer_data(0xFF);
		ps2_transfer_data(0xFF);
		ps2_transfer_data(0xFF);
		_delay_ms(1);
		PS2_PORT |= (1 << PS2_ATT);
		_delay_ms(10);

		
		// config mode verlassen
		PS2_PORT &= ~(1 << PS2_ATT);
		_delay_us(10);
		ps2_transfer_data(0x01);
		ps2_transfer_data(0x43);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x5A);
		ps2_transfer_data(0x5A);
		ps2_transfer_data(0x5A);
		ps2_transfer_data(0x5A);
		ps2_transfer_data(0x5A);
		_delay_ms(1);
		PS2_PORT |= (1 << PS2_ATT);
		_delay_ms(10);
		
		// poll zum prüfen
		PS2_PORT &= ~(1 << PS2_ATT);
		_delay_us(10);
		ps2_transfer_data(0x01);
		check = ps2_transfer_data(0x42);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		ps2_transfer_data(0x00);
		_delay_ms(1);
		PS2_PORT |= (1 << PS2_ATT);
		_delay_ms(10);
	} while (check != 0x73 && check != 0x41);
}

uint8_t ps2_poll()
{
	uint8_t check1, check2, i, mask;
	
	PS2_PORT &= ~(1 << PS2_ATT);
	_delay_us(1);
	ps2_transfer_data(0x01);
	check1 = ps2_transfer_data(0x42);		// controller mode muss 0x73 sein
	check2 = ps2_transfer_data(0x00);		// data start muss 0x5A sein
	if(check1 == 0x41)
	{
		ps2_data.btn1 = ps2_transfer_data(ps2_data.vib1);
		ps2_data.btn2 = ps2_transfer_data(ps2_data.vib2);
		ps2_data.rx = PS2_ANALOG_MID;	
		ps2_data.ry = PS2_ANALOG_MID;
		ps2_data.lx = PS2_ANALOG_MID;
		ps2_data.ly = PS2_ANALOG_MID;
	}
	else if(check1 == 0x73)
	{
		ps2_data.btn1 = ps2_transfer_data(ps2_data.vib1);
		ps2_data.btn2 = ps2_transfer_data(ps2_data.vib2);
		ps2_data.rx = ps2_transfer_data(0x00);
		ps2_data.ry = ps2_transfer_data(0x00);
		ps2_data.lx = ps2_transfer_data(0x00);
		ps2_data.ly = ps2_transfer_data(0x00);
	}
	PS2_PORT |= (1 << PS2_ATT);
	
	if((check1 == 0x73 || check1 == 0x41) && check2 == 0x5A)	// erfolg
	{
		mask = 1;
		ps2_data.btn1_down = 0;
		ps2_data.btn1_up = 0;
		ps2_data.btn2_down = 0;
		ps2_data.btn2_up = 0;
		
		for(i = 0; i < 8; i++)
		{
			if(!(ps2_data.btn1 & mask) && (ps2_data.last_btn1 & mask))	// jetzt gedrück, vorher nicht -> down
				ps2_data.btn1_down |= mask;
			else if((ps2_data.btn1 & mask) && !(ps2_data.last_btn1 & mask))	// jetzt nicht gedrück, vorher schon -> up
				ps2_data.btn1_up |= mask;
			
			if(!(ps2_data.btn2 & mask) && (ps2_data.last_btn2 & mask))	// jetzt gedrück, vorher nicht -> down
				ps2_data.btn2_down |= mask;
			else if((ps2_data.btn2 & mask) && !(ps2_data.last_btn2 & mask))	// jetzt nicht gedrück, vorher schon -> up
				ps2_data.btn2_up |= mask;
			
			mask <<= 1;
		}
		
		ps2_data.last_btn1 = ps2_data.btn1;
		ps2_data.last_btn2 = ps2_data.btn2;
		return 1;
	}
	
	return 0;
}

void ps2_set_vibration(uint8_t v1, uint8_t v2, uint8_t steps)
{
	if(v1)
		ps2_data.vib1 = 1;
	else
		ps2_data.vib1 = 0;
	
	ps2_data.vib2 = v2;
	ps2_vib_cnt = steps;
}

void ps2_vibration_step(void)
{
	if(ps2_vib_cnt)
	{
		ps2_vib_cnt--;
		if(!ps2_vib_cnt)
		{
			ps2_data.vib2 = 0;
			ps2_data.vib1 = 0;
		}
	}
}